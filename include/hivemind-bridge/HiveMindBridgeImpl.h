#ifndef HIVEMIND_BRIDGE_HIVEMINDBRIDGEIMPL_H
#define HIVEMIND_BRIDGE_HIVEMINDBRIDGEIMPL_H

#include "Callback.h"
#include "IUserCallRequestHandler.h"
#include "IUserCallbackMap.h"
#include "hivemind-bridge/IHiveMindBridge.h"
#include "hivemind-bridge/IHiveMindHostApiResponseHandler.h"
#include "hivemind-bridge/IThreadSafeQueue.h"
#include "hivemind-bridge/InboundRequestHandle.h"
#include "hivemind-bridge/MessageHandler.h"
#include "hivemind-bridge/OutboundRequestHandle.h"
#include "hivemind-bridge/TCPServer.h"
#include "pheromones/FunctionCallRequestDTO.h"
#include <cmath>
#include <cpp-common/ILogger.h>
#include <deque>
#include <memory>
#include <mutex>
#include <pheromones/BytesDTO.h>
#include <pheromones/HiveMindHostDeserializer.h>
#include <pheromones/HiveMindHostSerializer.h>
#include <thread>

constexpr int THREAD_SLEEP_MS = 1; // The sleep time of the trheads
constexpr int DELAY_BRFORE_DROP_S =
    10; // The maximum delay before which a request will be dropped if no response was received.

class HiveMindBridgeImpl {
  public:
    /**
     * Construct a HiveMind Bridge object by injecting already-initialised objects.
     * @param tcpServer A TCPServer to be used
     * @param serializer A HiveMindHostSerializer to be used
     * @param deserializer A HiveMindHostDeserializer to be used
     * @param userCallRequestManager A manager for the usercall request
     * @param hmRequestHandler A request handler
     * @param userCallbackMap A map of the usercallback
     * @param messageHandler A MessageHandler to be used
     * @param inboundQueue A ThreadSafeQueue to be  used for inbound messages
     * @param outboundQueue A ThreadSafeQueue to be  used for outbound request handles
     * @param logger The logger used in the bridge
     * @param keepAliveFrequency Number of call to spin before a greet is sent to keep the connection alive. Set to 0 to disable
     */
    HiveMindBridgeImpl(ITCPServer& tcpServer,
                       IHiveMindHostSerializer& serializer,
                       IHiveMindHostDeserializer& deserializer,
                       IUserCallRequestHandler& userCallRequestManager,
                       IHiveMindHostRequestHandler& hmRequestHandler,
                       IUserCallbackMap& userCallbackMap,
                       IMessageHandler& messageHandler,
                       IThreadSafeQueue<MessageDTO>& inboundQueue,
                       IThreadSafeQueue<OutboundRequestHandle>& outboundQueue,
                       ILogger& logger,
                       uint32_t keepAliveFrequency);

    ~HiveMindBridgeImpl();

    void spin();

    void onConnect(std::function<void()> hook);

    void onDisconnect(std::function<void()> hook);

    bool onBytesReceived(std::function<void(uint8_t* bytes, uint64_t bytesLength)> callback);

    bool registerCustomAction(std::string name,
                              CallbackFunction callback,
                              CallbackArgsManifest manifest);

    bool registerCustomAction(std::string name, CallbackFunction callback);

    bool queueAndSend(MessageDTO message);

    bool sendBytes(uint32_t destinationId, const uint8_t* const payload, uint16_t payloadSize);

    uint32_t getSwarmAgentId() const;

    bool sendNeighborUpdateRequest(uint16_t neighborId);

    bool sendNeighborListUpdateRequest();

  private:
    ILogger& m_logger;

    ITCPServer& m_tcpServer;
    IHiveMindHostDeserializer& m_deserializer;
    IHiveMindHostSerializer& m_serializer;
    IUserCallRequestHandler& m_userCallRequestHandler;
    IHiveMindHostRequestHandler& m_hmRequestHandler;

    IUserCallbackMap& m_userCallbackMap;
    IMessageHandler& m_messageHandler;

    IThreadSafeQueue<MessageDTO>& m_inboundQueue;
    std::thread m_inboundThread;
    IThreadSafeQueue<OutboundRequestHandle>& m_outboundQueue;
    std::thread m_outboundThread;
    std::mutex m_mutex;

    std::deque<InboundRequestHandle> m_inboundRequestsQueue;
    std::unordered_map<uint32_t, InboundResponseHandle> m_inboundResponsesMap;

    uint32_t m_swarmAgentID = 0;
    const uint32_t m_keepAliveFrequency;
    uint32_t m_keepAliveCounter = 0;

    void inboundThread();
    void outboundThread();
    bool isTCPClientConnected();
    void sendReturn(InboundRequestHandle& result);
    bool greet();
};

#endif // HIVEMIND_BRIDGE_HIVEMINDBRIDGEIMPL_H
