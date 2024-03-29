#include "hivemind-bridge/HiveMindBridge.h"

HiveMindBridge::HiveMindBridge(int tcpPort, ILogger& logger, uint32_t keepAliveNbSpin) :
    m_tcpServer(tcpPort, logger),
    m_deserializer(m_tcpServer),
    m_serializer(m_tcpServer),
    m_userCallRequestHandler(logger, m_userCallbackMap),
    m_hmRequestHandler(logger),
    m_hmResponseHandler(logger),
    m_messageHandler(logger, m_userCallRequestHandler, m_hmRequestHandler, m_hmResponseHandler),
    m_logger(logger),
    m_bridge(m_tcpServer,
             m_serializer,
             m_deserializer,
             m_userCallRequestHandler,
             m_hmRequestHandler,
             m_userCallbackMap,
             m_messageHandler,
             m_inboundQueue,
             m_outboundQueue,
             m_logger,
             keepAliveNbSpin) {}

void HiveMindBridge::spin() { m_bridge.spin(); }

void HiveMindBridge::onConnect(std::function<void()> hook) { m_bridge.onConnect(hook); }

void HiveMindBridge::onDisconnect(std::function<void()> hook) { m_bridge.onDisconnect(hook); }

bool HiveMindBridge::onBytesReceived(std::function<void(uint8_t*, uint64_t)> callback) {
    return m_bridge.onBytesReceived(callback);
}

bool HiveMindBridge::onNeighborListUpdated(
    std::function<void(std::array<uint16_t, NEIGHBORS_MAX_SIZE>, uint64_t bytesLength)> callback) {
    return m_hmResponseHandler.onNeighborListUpdated(callback);
}

bool HiveMindBridge::onNeighborUpdated(
    std::function<void(uint16_t neighborId, std::optional<Position> position)> callback) {
    return m_hmResponseHandler.onNeighborUpdated(callback);
}

bool HiveMindBridge::registerCustomAction(std::string name,
                                          CallbackFunction callback,
                                          CallbackArgsManifest manifest) {
    return m_bridge.registerCustomAction(name, callback, manifest);
}

bool HiveMindBridge::registerCustomAction(std::string name, CallbackFunction callback) {
    return m_bridge.registerCustomAction(name, callback);
}

bool HiveMindBridge::queueAndSend(MessageDTO message) { return m_bridge.queueAndSend(message); }

bool HiveMindBridge::sendBytes(uint32_t destinationId,
                               const uint8_t* const payload,
                               uint16_t payloadSize) {
    return m_bridge.sendBytes(destinationId, payload, payloadSize);
}

bool HiveMindBridge::sendNeighborUpdateRequest(uint16_t neighborId) {
    return m_bridge.sendNeighborUpdateRequest(neighborId);
}

bool HiveMindBridge::sendNeighborListUpdateRequest() {
    return m_bridge.sendNeighborListUpdateRequest();
}