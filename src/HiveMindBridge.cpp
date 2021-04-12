#include "HiveMindBridge/HiveMindBridge.h"

HiveMindBridge::HiveMindBridge(int tcpPort, ILogger& logger) :
    m_tcpServer(tcpPort, logger),
    m_deserializer(m_tcpServer),
    m_serializer(m_tcpServer),
    m_messageHandler(logger),
    m_logger(logger),
    m_bridge(m_tcpServer,
             m_serializer,
             m_deserializer,
             m_messageHandler,
             m_inboundQueue,
             m_outboundQueue,
             m_logger) {}

HiveMindBridge::~HiveMindBridge() {}

void HiveMindBridge::spin() { m_bridge.spin(); }

void HiveMindBridge::onConnect(std::function<void()> hook) { m_bridge.onConnect(hook); }

void HiveMindBridge::onDisconnect(std::function<void()> hook) { m_bridge.onDisconnect(hook); }

bool HiveMindBridge::registerCustomAction(std::string name,
                                          CallbackFunction callback,
                                          CallbackArgsManifest manifest) {
    return m_bridge.registerCustomAction(name, callback, manifest);
}

bool HiveMindBridge::registerCustomAction(std::string name, CallbackFunction callback) {
    return m_bridge.registerCustomAction(name, callback);
}

bool HiveMindBridge::queueAndSend(MessageDTO message) { return m_bridge.queueAndSend(message); }