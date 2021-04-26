#ifndef HIVEMIND_BRIDGE_HIVEMINDBRIDGE_H
#define HIVEMIND_BRIDGE_HIVEMINDBRIDGE_H
#include "hivemind-bridge/HiveMindBridgeImpl.h"
#include "hivemind-bridge/IHiveMindBridge.h"
#include "hivemind-bridge/MessageHandler.h"
#include "hivemind-bridge/OutboundRequestHandle.h"
#include "hivemind-bridge/TCPServer.h"
#include "hivemind-bridge/ThreadSafeQueue.h"
#include "UserCallRequestHandler.h"
#include "UserCallbackMap.h"
#include <cpp-common/ILogger.h>
#include <memory>
#include <pheromones/HiveMindHostDeserializer.h>
#include <pheromones/HiveMindHostSerializer.h>

class HiveMindBridge : public IHiveMindBridge {
  public:
    /**
     * Construct a HiveMindBridge
     * @param tcpPort The port that the TCP server should listen to
     */
    HiveMindBridge(int tcpPort, ILogger& logger);

    ~HiveMindBridge();

    void spin();

    void onConnect(std::function<void()> hook);

    void onDisconnect(std::function<void()> hook);

    bool registerCustomAction(std::string name,
                              CallbackFunction callback,
                              CallbackArgsManifest manifest);

    bool registerCustomAction(std::string name, CallbackFunction callback);

    bool queueAndSend(MessageDTO message);

    bool sendBytes(uint32_t destinationId, const uint8_t* const payload, uint16_t payloadSize);

  private:
    ILogger& m_logger;
    TCPServer m_tcpServer;
    HiveMindHostDeserializer m_deserializer;
    HiveMindHostSerializer m_serializer;
    UserCallbackMap m_userCallbackMap;
    UserCallRequestHandler m_userCallRequestHandler;
    MessageHandler m_messageHandler;
    ThreadSafeQueue<MessageDTO> m_inboundQueue;
    ThreadSafeQueue<OutboundRequestHandle> m_outboundQueue;
    HiveMindBridgeImpl m_bridge;
};

#endif // HIVEMIND_BRIDGE_HIVEMINDBRIDGE_H
