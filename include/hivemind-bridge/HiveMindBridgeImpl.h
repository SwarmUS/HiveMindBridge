#ifndef HIVEMIND_BRIDGE_HIVEMINDBRIDGEIMPL_H
#define HIVEMIND_BRIDGE_HIVEMINDBRIDGEIMPL_H

#include "hivemind-bridge/IHiveMindBridge.h"
#include "hivemind-bridge/IThreadSafeQueue.h"
#include "hivemind-bridge/InboundRequestHandle.h"
#include "hivemind-bridge/MessageHandler.h"
#include "hivemind-bridge/OutboundRequestHandle.h"
#include "hivemind-bridge/TCPServer.h"
#include "hivemind-bridge/user-call/Callback.h"
#include "hivemind-bridge/user-call/IUserCallRequestManager.h"
#include "hivemind-bridge/user-call/IUserCallbackMap.h"
#include <cmath>
#include <cpp-common/ILogger.h>
#include <deque>
#include <memory>
#include <mutex>
#include <pheromones/BytesDTO.h>
#include <pheromones/HiveMindHostDeserializer.h>
#include <pheromones/HiveMindHostSerializer.h>
#include <thread>

constexpr int THREAD_SLEEP_MS = 250; // The sleep time of the trheads
constexpr int DELAY_BRFORE_DROP_S =
    10; // The maximum delay before which a request will be dropped if no response was received.

class HiveMindBridgeImpl : public IHiveMindBridge {
  public:
    /**
     * Construct a HiveMind Bridge object by injecting already-initialised objects.
     * @param tcpServer A TCPServer to be used
     * @param serializer A HiveMindHostSerializer to be used
     * @param messageHandler A MessageHandler to be used
     * @param deserializer A HiveMindHostDeserializer to be used
     * @param inboundQueue A ThreadSafeQueue to be  used
     */
    HiveMindBridgeImpl(ITCPServer& tcpServer,
                       IHiveMindHostSerializer& serializer,
                       IHiveMindHostDeserializer& deserializer,
                       IUserCallRequestManager& userCallRequestManager,
                       IUserCallbackMap& userCallbackMap,
                       IMessageHandler& messageHandler,
                       IThreadSafeQueue<MessageDTO>& inboundQueue,
                       IThreadSafeQueue<OutboundRequestHandle>& outboundQueue,
                       ILogger& logger);

    ~HiveMindBridgeImpl();

    void spin() override;

    void onConnect(std::function<void()> hook) override;

    void onDisconnect(std::function<void()> hook) override;

    bool registerCustomAction(std::string name,
                              CallbackFunction callback,
                              CallbackArgsManifest manifest) override;

    bool registerCustomAction(std::string name, CallbackFunction callback) override;

    bool queueAndSend(MessageDTO message) override;

    bool sendBytes(uint32_t destinationId, const uint8_t* const payload, uint16_t payloadSize);

    uint32_t getSwarmAgentId();

  private:
    ILogger& m_logger;

    ITCPServer& m_tcpServer;
    IHiveMindHostDeserializer& m_deserializer;
    IHiveMindHostSerializer& m_serializer;
    IUserCallRequestManager& m_userCallRequestHandler;
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

    void inboundThread();
    void outboundThread();
    bool isTCPClientConnected();
    void sendReturn(InboundRequestHandle result);
    bool greet();
};

#endif // HIVEMIND_BRIDGE_HIVEMINDBRIDGEIMPL_H
