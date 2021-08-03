#ifndef HIVEMIND_BRIDGE_HIVEMINDBRIDGE_H
#define HIVEMIND_BRIDGE_HIVEMINDBRIDGE_H
#include "UserCallRequestHandler.h"
#include "UserCallbackMap.h"
#include "hivemind-bridge/BytesAccumulator.h"
#include "hivemind-bridge/HiveMindBridgeImpl.h"
#include "hivemind-bridge/HiveMindHostApiRequestHandler.h"
#include "hivemind-bridge/HiveMindHostApiResponseHandler.h"
#include "hivemind-bridge/IHiveMindBridge.h"
#include "hivemind-bridge/MessageHandler.h"
#include "hivemind-bridge/OutboundRequestHandle.h"
#include "hivemind-bridge/TCPServer.h"
#include "hivemind-bridge/ThreadSafeQueue.h"
#include <cpp-common/ILogger.h>
#include <memory>
#include <pheromones/HiveMindHostDeserializer.h>
#include <pheromones/HiveMindHostSerializer.h>

class HiveMindBridge : public IHiveMindBridge {
  public:
    /**
     * Construct a HiveMindBridge
     * @param tcpPort The port that the TCP server should listen to
     * @param logger logger used
     */
    HiveMindBridge(int tcpPort, ILogger& logger);

    ~HiveMindBridge() = default;

    void spin();

    void onConnect(std::function<void()> hook);

    void onDisconnect(std::function<void()> hook);

    bool onBytesReceived(
        std::function<void(uint8_t* bytes, uint64_t bytesLength)> callback) override;

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
    HiveMindHostApiRequestHandler m_hmRequestHandler;
    HiveMindHostApiResponseHandler m_hmResponseHandler;
    MessageHandler m_messageHandler;
    ThreadSafeQueue<MessageDTO> m_inboundQueue;
    ThreadSafeQueue<OutboundRequestHandle> m_outboundQueue;
    HiveMindBridgeImpl m_bridge;
};

#endif // HIVEMIND_BRIDGE_HIVEMINDBRIDGE_H
