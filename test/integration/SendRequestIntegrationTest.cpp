#include "../utils/Logger.h"
#include "../utils/TCPClient.h"
#include "../utils/HiveMindBridgeFixture.h"
#include "../utils/BytesTestData.h"
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

class SendRequestIntegrationTestFixture : public testing::Test, public HiveMindBridgeFixture {

    void SetUp() { std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_DELAY_MS)); }

    void TearDown(){
        cleanUpAfterTest();
    };

public:
    SendRequestIntegrationTestFixture() {
    }

    ~SendRequestIntegrationTestFixture() {
    }

    // Teardown method that needs to be run manually since we run everything inside a single test case.
    void cleanUpAfterTest() {

    }

    void testQueueAndSend() {
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

    void testSendBytes() {
        // Given
        m_bridge->sendBytes(CLIENT_AGENT_ID, LONG_BYTE_ARRAY.arr, LONG_BYTE_ARRAY_SIZE);
        int expectedNumberOfPackets = std::ceil((float) LONG_BYTE_ARRAY_SIZE / BYTES_PAYLOAD_SIZE);

        // When
        for (int i = 0; i < expectedNumberOfPackets; i++) {
            MessageDTO message;
            m_clientDeserializer->deserializeFromStream(message);

            RequestDTO request = std::get<RequestDTO>(message.getMessage());
            HiveMindHostApiRequestDTO hmRequest = std::get<HiveMindHostApiRequestDTO>(request.getRequest());
            BytesDTO bytes = std::get<BytesDTO>(hmRequest.getRequest());

            ASSERT_EQ(bytes.getPacketNumber(), i);
        }

        // Then

        cleanUpAfterTest();
    }

};

TEST_F(SendRequestIntegrationTestFixture, testUserCallbacks) {
    testQueueAndSend();
    testSendBytes();
}
