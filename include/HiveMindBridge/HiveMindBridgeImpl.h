#ifndef HIVEMIND_BRIDGE_HIVEMINDBRIDGEIMPL_H
#define HIVEMIND_BRIDGE_HIVEMINDBRIDGEIMPL_H

#include "HiveMindBridge/Callback.h"
#include "HiveMindBridge/IHiveMindBridge.h"
#include "HiveMindBridge/IThreadSafeQueue.h"
#include "HiveMindBridge/InboundRequestHandle.h"
#include "HiveMindBridge/MessageHandler.h"
#include "HiveMindBridge/OutboundRequestHandle.h"
#include "HiveMindBridge/TCPServer.h"
#include <cpp-common/ILogger.h>
#include <deque>
#include <hivemind-host/HiveMindHostDeserializer.h>
#include <hivemind-host/HiveMindHostSerializer.h>
#include <memory>
#include <mutex>
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

    uint32_t getSwarmAgentId();

  private:
    ILogger& m_logger;

    ITCPServer& m_tcpServer;
    IHiveMindHostDeserializer& m_deserializer;
    IHiveMindHostSerializer& m_serializer;
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