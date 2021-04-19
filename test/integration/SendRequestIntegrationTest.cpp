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

    void sendAck(uint32_t sourceId, uint32_t destinationId, uint32_t requestId) {
        GenericResponseDTO genericResponse(GenericResponseStatusDTO::Ok, "");
        ResponseDTO response(requestId, genericResponse);
        MessageDTO ackMessage(sourceId, destinationId, response);
        m_clientSerializer->serializeToStream(ackMessage);
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
            sendAck(message.getDestinationId(), message.getSourceId(), request.getId());
        }

        cleanUpAfterTest();
    }

    BytesDTO getBytesRequestFromStream() {
        MessageDTO message;
        m_clientDeserializer->deserializeFromStream(message);

        RequestDTO request = std::get<RequestDTO>(message.getMessage());
        HiveMindHostApiRequestDTO hmRequest = std::get<HiveMindHostApiRequestDTO>(request.getRequest());

        sendAck(message.getDestinationId(), message.getSourceId(), request.getId());

        return std::get<BytesDTO>(hmRequest.getRequest());
    }

    void testSendBytes() {
        // Given
        m_bridge->sendBytes(CLIENT_AGENT_ID, LONG_BYTE_ARRAY.arr, LONG_BYTE_ARRAY_SIZE);
        int expectedNumberOfPackets = std::ceil((float) LONG_BYTE_ARRAY_SIZE / BYTES_PAYLOAD_SIZE);

        // When, then
        std::this_thread::sleep_for(std::chrono::milliseconds(7 * THREAD_DELAY_MS));

        // Check first packet
        uint32_t bytesReqId = 0;
        {
            BytesDTO bytes = getBytesRequestFromStream();
            bytesReqId = bytes.getPacketId();

            ASSERT_EQ(bytes.getPacketNumber(), 0);
            ASSERT_FALSE(bytes.isLastPacket());
        }

        // Check packets in the middle
        for (int i = 1; i < expectedNumberOfPackets - 1; i++) {
            BytesDTO bytes = getBytesRequestFromStream();

            ASSERT_EQ(bytes.getPacketNumber(), i);
            ASSERT_EQ(bytes.getPacketId(), bytesReqId);
            ASSERT_FALSE(bytes.isLastPacket());
        }

        // Check last packet
        {
            BytesDTO bytes = getBytesRequestFromStream();

            ASSERT_EQ(bytes.getPacketNumber(), expectedNumberOfPackets - 1);
            ASSERT_EQ(bytes.getPacketId(), bytesReqId);
            ASSERT_TRUE(bytes.isLastPacket());
        }

        cleanUpAfterTest();
    }

};

TEST_F(SendRequestIntegrationTestFixture, testUserCallbacks) {
    testSendBytes();
    testQueueAndSend();
}
