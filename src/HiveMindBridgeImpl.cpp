#include "hivemind-bridge/HiveMindBridgeImpl.h"
#include "pheromones/GetNeighborRequestDTO.h"
#include "pheromones/GetNeighborsListRequestDTO.h"

HiveMindBridgeImpl::HiveMindBridgeImpl(ITCPServer& tcpServer,
                                       IHiveMindHostSerializer& serializer,
                                       IHiveMindHostDeserializer& deserializer,
                                       IUserCallRequestHandler& userCallRequestManager,
                                       IHiveMindHostRequestHandler& hmRequestHandler,
                                       IUserCallbackMap& userCallbackMap,
                                       IMessageHandler& messageHandler,
                                       IThreadSafeQueue<MessageDTO>& inboundQueue,
                                       IThreadSafeQueue<OutboundRequestHandle>& outboundQueue,
                                       ILogger& logger,
                                       uint32_t keepAliveFrequency) :
    m_tcpServer(tcpServer),
    m_serializer(serializer),
    m_deserializer(deserializer),
    m_userCallRequestHandler(userCallRequestManager),
    m_hmRequestHandler(hmRequestHandler),
    m_userCallbackMap(userCallbackMap),
    m_messageHandler(messageHandler),
    m_inboundQueue(inboundQueue),
    m_outboundQueue(outboundQueue),
    m_logger(logger),
    m_keepAliveFrequency(keepAliveFrequency) {}

HiveMindBridgeImpl::~HiveMindBridgeImpl() {
    m_tcpServer.close();

    if (m_inboundThread.joinable()) {
        m_inboundThread.join();
    }

    if (m_outboundThread.joinable()) {
        m_outboundThread.join();
    }
}

void HiveMindBridgeImpl::spin() {
    if (isTCPClientConnected()) {
        if (!m_inboundQueue.empty()) {
            auto vHandle = m_messageHandler.handleMessage(m_inboundQueue.front());
            m_inboundQueue.pop();

            if (std::holds_alternative<InboundRequestHandle>(vHandle)) {
                InboundRequestHandle handle = std::get<InboundRequestHandle>(vHandle);
                m_inboundRequestsQueue.push_back(handle);

                // Send the ack/nack message
                m_serializer.serializeToStream(handle.getResponse());
            } else if (std::holds_alternative<InboundResponseHandle>(vHandle)) {
                InboundResponseHandle handle = std::get<InboundResponseHandle>(vHandle);
                m_inboundResponsesMap[handle.getResponseId()] = handle;
            }
        }

        for (auto result = m_inboundRequestsQueue.begin();
             result != m_inboundRequestsQueue.end();) {

            if (result->getCallbackReturnContext().valid()) {

                // Check if async function has finished running
                if (result->getCallbackReturnContext().wait_for(std::chrono::seconds(0)) ==
                    std::future_status::ready) {
                    // Finished, so we send the return payload
                    sendReturn(*result);

                    // update the result since erase() returns the iterator of the next value.
                    result = m_inboundRequestsQueue.erase(result);
                } else {
                    // Not finished running, skip to the next
                    result++;
                }
            } else {
                result = m_inboundRequestsQueue.erase(result);
            }
        }
    } else {
        if (m_inboundThread.joinable()) {
            m_inboundThread.join();
        }

        if (m_outboundThread.joinable()) {
            m_outboundThread.join();
        }

        m_tcpServer.listen();

        if (greet()) {
            m_inboundThread = std::thread(&HiveMindBridgeImpl::inboundThread, this);
            m_outboundThread = std::thread(&HiveMindBridgeImpl::outboundThread, this);
        } else {
            m_tcpServer.close();
        }
    }
    if(m_keepAliveFrequency > 0){
        if(m_keepAliveCounter >= m_keepAliveFrequency){
            m_keepAliveCounter = 0;
            MessageDTO message = MessageUtils::createGreetMessage();
            queueAndSend(message);
        }
        m_keepAliveCounter++;
    }

}

void HiveMindBridgeImpl::onConnect(std::function<void()> hook) {
    m_tcpServer.onConnect(std::move(hook));
}

void HiveMindBridgeImpl::onDisconnect(std::function<void()> hook) {
    m_tcpServer.onDisconnect(std::move(hook));
}

bool HiveMindBridgeImpl::onBytesReceived(std::function<void(uint8_t*, uint64_t)> callback) {
    return m_hmRequestHandler.onBytesReceived(std::move(callback));
}

bool HiveMindBridgeImpl::registerCustomAction(std::string name,
                                              CallbackFunction callback,
                                              CallbackArgsManifest manifest) {
    return m_userCallbackMap.registerCallback(std::move(name), std::move(callback),
                                              std::move(manifest));
}

bool HiveMindBridgeImpl::registerCustomAction(std::string name, CallbackFunction callback) {
    return m_userCallbackMap.registerCallback(std::move(name), std::move(callback));
}

bool HiveMindBridgeImpl::queueAndSend(MessageDTO message) {
    if (isTCPClientConnected()) {
        OutboundRequestHandle handle(message);
        m_outboundQueue.push(handle);
        return true;
    }

    return false;
}

bool HiveMindBridgeImpl::sendBytes(uint32_t destinationId,
                                   const uint8_t* const payload,
                                   uint16_t payloadSize) {
    int nPackets = std::ceil((float)payloadSize / BytesDTO::PAYLOAD_MAX_SIZE);
    uint32_t bytesReqId = MessageUtils::generateRandomId();

    for (int packetNumber = 0; packetNumber < nPackets; packetNumber++) {
        bool isLastPacket = packetNumber == nPackets - 1;
        uint8_t packetSize = BytesDTO::PAYLOAD_MAX_SIZE;
        uint16_t packetStartIndex = packetNumber * BytesDTO::PAYLOAD_MAX_SIZE;

        if (isLastPacket) {
            isLastPacket = true;
            packetSize = (payloadSize - packetNumber * BytesDTO::PAYLOAD_MAX_SIZE) %
                         BytesDTO::PAYLOAD_MAX_SIZE;
        }

        uint8_t* packetStartPtr = (uint8_t*)payload + packetStartIndex;

        if (auto msg = MessageUtils::createBytesMessage(
                m_swarmAgentID, destinationId, MessageUtils::generateRandomId(), bytesReqId,
                packetNumber, isLastPacket, packetStartPtr, packetSize)) {
            if (!queueAndSend(msg.value())) {
                m_logger.log(LogLevel::Error, "Sending bytes failed");
                return false;
            }
        } else {
            m_logger.log(LogLevel::Error, "Bytes message creation failed");
            return false;
        }
    }

    return true;
}

uint32_t HiveMindBridgeImpl::getSwarmAgentId() const { return m_swarmAgentID; }

void HiveMindBridgeImpl::inboundThread() {
    while (isTCPClientConnected()) {
        MessageDTO message;
        if (m_deserializer.deserializeFromStream(message)) {
            m_inboundQueue.push(message);
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_SLEEP_MS));
}

void HiveMindBridgeImpl::outboundThread() {
    while (isTCPClientConnected()) {
        if (!m_outboundQueue.empty()) {
            OutboundRequestHandle handle = m_outboundQueue.front();

            MessageDTO outboundMessage = handle.getMessage();

            if (const auto* request = std::get_if<RequestDTO>(&outboundMessage.getMessage())) {
                // verify if the front value has a corresponding inbound response handle
                auto search = m_inboundResponsesMap.find(request->getId());
                if (search != m_inboundResponsesMap.end()) {
                    // Received a response for this request so we delete the request
                    // TODO add some retry logic in case the response was not ok
                    m_outboundQueue.pop();
                    m_inboundResponsesMap.erase(search);
                    m_logger.log(LogLevel::Debug, "RECEIVED VALID RESPONSE");
                } else {
                    // Did not receive a response for this request. Was the request sent?
                    if (handle.getState() == OutboundRequestState::READY) {
                        m_serializer.serializeToStream(outboundMessage);
                        handle.setState(OutboundRequestState::SENT);

                        m_outboundQueue.pop();
                        m_outboundQueue.push(handle); // Cycle through the queue
                    } else {
                        // Drop message or cycle through queue
                        if (handle.bumpDelaySinceSent(THREAD_SLEEP_MS) >= DELAY_BRFORE_DROP_S) {
                            // TODO add some retry logic after a timeout. For now, we simply
                            // drop.
                            m_outboundQueue.pop();
                        } else {
                            m_outboundQueue.pop();
                            m_outboundQueue.push(handle); // Cycle through the queue
                        }
                    }
                }
            } else if(std::get_if<GreetingDTO>(&outboundMessage.getMessage())){
                m_serializer.serializeToStream(outboundMessage);

            } else {
                m_logger.log(LogLevel::Warn, "Outbound queue contains an unsupported message");
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_SLEEP_MS));
    }
}

bool HiveMindBridgeImpl::isTCPClientConnected() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_tcpServer.isClientConnected();
}

void HiveMindBridgeImpl::sendReturn(InboundRequestHandle& result) {
    std::optional<CallbackReturn> callbackReturnOpt = result.getCallbackReturnContext().get();

    // Send a return only if there is a return value
    if (callbackReturnOpt.has_value()) {
        CallbackArgs args = callbackReturnOpt.value().getReturnArgs();

        if (args.size() > FunctionCallRequestDTO::FUNCTION_CALL_ARGUMENTS_MAX_LENGTH) {
            m_logger.log(LogLevel::Error,
                         "Return function contains too many arguments."
                         "The limit is currently set to %d arguments",
                         FunctionCallRequestDTO::FUNCTION_CALL_ARGUMENTS_MAX_LENGTH);

            return;
        }

        MessageDTO returnMessage = MessageUtils::createFunctionCallRequest(
            result.getMessageDestinationId(), // swap source and dest since we
            // return to the sender
            result.getMessageSourceId(), MessageUtils::generateRandomId(),
            result.getSourceModule(), // swap source and dest since we return to the
            // sender
            callbackReturnOpt.value().getReturnFunctionName(), args);

        m_serializer.serializeToStream(returnMessage);
    }
}

bool HiveMindBridgeImpl::greet() {
    if (isTCPClientConnected()) {
        MessageDTO message = MessageUtils::createGreetMessage();
        m_serializer.serializeToStream(message);

        MessageDTO greetResponse;
        m_deserializer.deserializeFromStream(greetResponse);

        std::optional<uint32_t> swarmAgentId = m_messageHandler.handleGreet(greetResponse);

        if (swarmAgentId.has_value()) {
            m_swarmAgentID = swarmAgentId.value();
            return true;
        }
    }

    return false;
}

bool HiveMindBridgeImpl::sendNeighborUpdateRequest(uint16_t neighborId) {
    GetNeighborRequestDTO neighborReq(neighborId);
    HiveMindHostApiRequestDTO hmReq(neighborReq);
    RequestDTO req(MessageUtils::generateRandomId(), hmReq);
    MessageDTO msg(getSwarmAgentId(), getSwarmAgentId(), req);

    return queueAndSend(msg);
}

bool HiveMindBridgeImpl::sendNeighborListUpdateRequest() {
    GetNeighborsListRequestDTO neighborReq;
    HiveMindHostApiRequestDTO hmReq(neighborReq);
    RequestDTO req(MessageUtils::generateRandomId(), hmReq);
    MessageDTO msg(getSwarmAgentId(), getSwarmAgentId(), req);

    return queueAndSend(msg);
}