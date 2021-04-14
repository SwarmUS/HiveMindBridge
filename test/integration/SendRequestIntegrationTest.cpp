#include "../utils/Logger.h"
#include "../utils/TCPClient.h"
#include "../utils/HiveMindBridgeFixture.h"
#include "HiveMindBridge/HiveMindBridge.h"
#include <gmock/gmock.h>
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

class UserCallbackIntegrationTestFixture : public testing::Test, public HiveMindBridgeFixture {
protected:
    Logger m_logger;
    int m_tcpPort = 5001;

// Bridge side
    HiveMindBridge* m_bridge;

// Client side
    TCPClient* m_tcpClient;
    HiveMindHostSerializer* m_clientSerializer;
    HiveMindHostDeserializer* m_clientDeserializer;

    void setUpCallbacks() {
    }

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

    void SetUp() { std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_DELAY_MS)); }

    void TearDown(){
        cleanUpAfterTest();
    };

public:
    UserCallbackIntegrationTestFixture() {
        // Bridge side
        m_bridge = new HiveMindBridge(m_tcpPort, m_logger);
        m_bridgeThread =
                std::thread(&UserCallbackIntegrationTestFixture::bridgeThread, this);

        setUpCallbacks();

        // Client side
        m_tcpClient = new TCPClient(m_tcpPort);
        m_clientSerializer = new HiveMindHostSerializer(*m_tcpClient);
        m_clientDeserializer = new HiveMindHostDeserializer(*m_tcpClient);

        connectClient();

        greet();
    }

    ~UserCallbackIntegrationTestFixture() {
        g_threadShouldRun = false;
        m_bridgeThread.join();
        m_tcpClient->close();

        delete m_tcpClient;
        delete m_clientSerializer;
        delete m_clientDeserializer;
        delete m_bridge;
    }

    std::thread m_bridgeThread;

    void bridgeThread() {
        while (g_threadShouldRun) {
            m_bridge->spin();
            std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_DELAY_MS));
        }
    }

    // Teardown method that needs to be run manually since we run everything inside a single test case.
    void cleanUpAfterTest() {

    }

    void testQueueSandSend() {
        for (int i = 0; i < 5; i++) {
            // Given
            m_bridge->queueAndSend(MessageUtils::createFunctionCallRequest(
                    CLIENT_AGENT_ID, CLIENT_AGENT_ID, 586, UserCallTargetDTO::UNKNOWN, "someRemoteCallback"));

            // When
            // Listen for request
            MessageDTO message;
            m_clientDeserializer->deserializeFromStream(message);

            // Then
            RequestDTO request = std::get<RequestDTO>(message.getMessage());
            UserCallRequestDTO userCallRequest = std::get<UserCallRequestDTO>(request.getRequest());
            FunctionCallRequestDTO functionCallRequest =
                    std::get<FunctionCallRequestDTO>(userCallRequest.getRequest());
            std::string functionName = functionCallRequest.getFunctionName();

            ASSERT_STREQ(functionName.c_str(), "someRemoteCallback");

            // Send ack
            GenericResponseDTO genericResponse(GenericResponseStatusDTO::Ok, "");
            ResponseDTO response(request.getId(), genericResponse);
            MessageDTO ackMessage(message.getDestinationId(), message.getSourceId(), response);
            m_clientSerializer->serializeToStream(ackMessage);
        }

        cleanUpAfterTest();
    }

};

TEST_F(UserCallbackIntegrationTestFixture, testUserCallbacks) {
    testQueueSandSend();
}
