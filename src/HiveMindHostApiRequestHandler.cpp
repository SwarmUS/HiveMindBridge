#include "hivemind-bridge/HiveMindHostApiRequestHandler.h"

HiveMindHostApiRequestHandler::HiveMindHostApiRequestHandler(ILogger& logger) :
    m_logger(logger) {}

void HiveMindHostApiRequestHandler::handleMessage(const MessageDTO& message,
                                                  const HiveMindHostApiRequestDTO& hmRequest) {

    if (const auto* bytesRequest = std::get_if<BytesDTO>(&hmRequest.getRequest())) {
        handleBytes(message, *bytesRequest);
    } else {
        m_logger.log(LogLevel::Warn, "Unsupported HiveMindHostApi request.");
    }
}

bool HiveMindHostApiRequestHandler::onBytesReceived(
    std::function<void(uint8_t* bytes, uint64_t bytesLength)> callback) {
    bool retVal = false;
    if (m_bytesReceivedCallback) {
        retVal = true;
    };

    m_bytesReceivedCallback = callback;

    return retVal;
}

void HiveMindHostApiRequestHandler::handleBytes(const MessageDTO& message, const BytesDTO& bytes) {
    (void)message;

    if (!m_bytesReceivedCallback) {
        m_logger.log(LogLevel::Error, "No callback was defined to handle incoming bytes.");
        return;
    }

    uint32_t packetId = bytes.getPacketId();

    // Create new accumulator for new requests
    if (m_bytesAccumulatorMap.find(packetId) == m_bytesAccumulatorMap.end()) {
        m_bytesAccumulatorMap.emplace(packetId, BytesAccumulator());
    }

    // Append the data to the accumulator
    bool success = m_bytesAccumulatorMap[packetId].appendBytes(const_cast<uint8_t*>(bytes.getPayload().data()),
                                                bytes.getPayloadLength(), bytes.getPacketNumber());

    if (!success) {
        m_logger.log(LogLevel::Error, "A packet was skipped.");
        return;
    }

    // Invoke the callback
    if (bytes.isLastPacket()) {
        m_bytesReceivedCallback(m_bytesAccumulatorMap[packetId].getBytes().data(),
                                m_bytesAccumulatorMap[packetId].getBytes().size());
        m_bytesAccumulatorMap.erase(m_bytesAccumulatorMap.find(packetId));
    }
}