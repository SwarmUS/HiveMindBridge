
#include "../utils/HiveMindBridgeFixture.h"
#include "../utils/Logger.h"
#include "../utils/TCPClient.h"
#include "hivemind-bridge/HiveMindBridge.h"
#include <atomic>
#include <gmock/gmock.h>
#include <pheromones/FunctionCallArgumentDTO.h>
#include <pheromones/FunctionCallRequestDTO.h>
#include <pheromones/HiveMindHostDeserializer.h>
#include <pheromones/HiveMindHostSerializer.h>
#include <pheromones/MessageDTO.h>
#include <pheromones/RequestDTO.h>
#include <pheromones/UserCallRequestDTO.h>
#include <thread>

// Global variables to test side effects from callbacks
bool g_functionCalled = false;

class ReceiveBytesIntegrationTestFixture : public testing::Test, public HiveMindBridgeFixture {
protected:
    void SetUp() {
        std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_DELAY_MS));
    }

    void TearDown() { cleanUpAfterTest(); };

public:
    // Teardown method that needs to be run manually since we run everything inside a single test
    // case.
    void cleanUpAfterTest() {
        g_functionCalled = false;
    }

    void testReceiveShortPayload() {
        // Given
        uint32_t byteReqId = 12;
        uint8_t payload[] = {1, 2, 3, 4};

        BytesDTO bytes0(byteReqId, 0, true, payload, 4);
        HiveMindHostApiRequestDTO hmReq0(bytes0);
        RequestDTO request0(42, hmReq0);
        MessageDTO message0(0, 0, request0);

        m_bridge->onBytesReceived([&](uint8_t* bytes, uint64_t bytesLength){
            g_functionCalled = true;
            uint8_t expectedBytes[] = {1, 2, 3, 4};

            ASSERT_EQ(bytesLength, 4);
            ASSERT_EQ(memcmp(bytes, expectedBytes, 4), 0);
        });

        // When
        m_clientSerializer->serializeToStream(message0);

        // Then
        std::this_thread::sleep_for(std::chrono::milliseconds(2 * THREAD_DELAY_MS));
        ASSERT_TRUE(g_functionCalled);

        cleanUpAfterTest();
    }

    void testReceiveLongPayload() {
        // Given
        uint32_t byteReqId = 13;
        uint8_t payload[] = {1, 2, 3, 4};

        BytesDTO bytes0(byteReqId, 0, false, payload, 4);
        HiveMindHostApiRequestDTO hmReq0(bytes0);
        RequestDTO request0(42, hmReq0);
        MessageDTO message0(0, 0, request0);

        BytesDTO bytes1(byteReqId, 1, false, payload, 4);
        HiveMindHostApiRequestDTO hmReq1(bytes1);
        RequestDTO request1(42, hmReq1);
        MessageDTO message1(0, 0, request1);

        BytesDTO bytes2(byteReqId, 2, false, payload, 4);
        HiveMindHostApiRequestDTO hmReq2(bytes2);
        RequestDTO request2(42, hmReq0);
        MessageDTO message2(0, 0, request0);

        BytesDTO bytesLastMsg(byteReqId, 3, true, payload, 4);
        HiveMindHostApiRequestDTO hmReqLastMsg(bytesLastMsg);
        RequestDTO requestLastMsg(42, hmReqLastMsg);
        MessageDTO incomingLastMsg(0, 0, requestLastMsg);

        m_bridge->onBytesReceived([&](uint8_t* bytes, uint64_t bytesLength) {
            g_functionCalled = true;
            uint8_t expectedBytes[] = {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4};

            ASSERT_EQ(bytesLength, 16);
            ASSERT_EQ(memcmp(bytes, expectedBytes, 16), 0);
        });

        // When
        m_clientSerializer->serializeToStream(message0);
        m_clientSerializer->serializeToStream(message1);
        m_clientSerializer->serializeToStream(message2);
        m_clientSerializer->serializeToStream(incomingLastMsg);

        // Then
        std::this_thread::sleep_for(std::chrono::milliseconds(2 * THREAD_DELAY_MS));
        ASSERT_TRUE(g_functionCalled);

        cleanUpAfterTest();
    }
};

TEST_F(ReceiveBytesIntegrationTestFixture, testUserCallbacks) {
    testReceiveShortPayload();
    testReceiveLongPayload();
}
