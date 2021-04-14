#ifndef HIVEMINDBRIDGE_HIVEMINDBRIDGEFIXTURE_H
#define HIVEMINDBRIDGE_HIVEMINDBRIDGEFIXTURE_H

#include "HiveMindBridge/HiveMindBridge.h"
#include <pheromones/HiveMindHostDeserializer.h>
#include <pheromones/HiveMindHostSerializer.h>
#include <pheromones/MessageDTO.h>
#include <pheromones/RequestDTO.h>
#include <pheromones/UserCallRequestDTO.h>
#include <pheromones/FunctionCallArgumentDTO.h>
#include <pheromones/FunctionCallRequestDTO.h>
#include <thread>
#include <atomic>

std::atomic_bool g_threadShouldRun = true;

constexpr uint32_t CLIENT_AGENT_ID = 12;
constexpr int THREAD_DELAY_MS = 100;

class HiveMindBridgeFixture {
protected:
    Logger m_logger;
    int m_tcpPort = 5001;
    std::thread m_bridgeThread;

    // Bridge side
    HiveMindBridge* m_bridge;

    // Client side
    TCPClient* m_tcpClient;
    HiveMindHostSerializer* m_clientSerializer;
    HiveMindHostDeserializer* m_clientDeserializer;

    void connectClient() {
        std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_DELAY_MS));
        m_tcpClient->connect();
    }

    void greet() {
        // Wait for a greet message
        MessageDTO greetRequest;
        if (m_clientDeserializer->deserializeFromStream(greetRequest)) {
            MessageDTO greetResponse(CLIENT_AGENT_ID, CLIENT_AGENT_ID, GreetingDTO(CLIENT_AGENT_ID));
            m_clientSerializer->serializeToStream(greetResponse);
        } else {
            m_logger.log(LogLevel::Warn, "Deserializing greet failed.");
        }
    }

    void bridgeThread() {
        while (g_threadShouldRun) {
            m_bridge->spin();
            std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_DELAY_MS));
        }
    }

public:
    HiveMindBridgeFixture() {
        // Bridge side
        m_bridge = new HiveMindBridge(m_tcpPort, m_logger);
        m_bridgeThread =
                std::thread(&HiveMindBridgeFixture::bridgeThread, this);

        // Client side
        m_tcpClient = new TCPClient(m_tcpPort);
        m_clientSerializer = new HiveMindHostSerializer(*m_tcpClient);
        m_clientDeserializer = new HiveMindHostDeserializer(*m_tcpClient);

        connectClient();

        greet();
    }

    ~HiveMindBridgeFixture() {
        g_threadShouldRun = false;
        m_bridgeThread.join();
        m_tcpClient->close();

        delete m_tcpClient;
        delete m_clientSerializer;
        delete m_clientDeserializer;
        delete m_bridge;
    }
};

#endif //HIVEMINDBRIDGE_HIVEMINDBRIDGEFIXTURE_H
