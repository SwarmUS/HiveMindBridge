
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
int g_functionCalledCount = 0;

class ReceiveBytesIntegrationTestFixture : public testing::Test, public HiveMindBridgeFixture {
  protected:
    void SetUp() override {
        std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_DELAY_MS));
    }

    void TearDown() override { cleanUpAfterTest(); };

  public:
    // Teardown method that needs to be run manually since we run everything inside a single test
    // case.
    static void cleanUpAfterTest() { g_functionCalledCount = 0; }

    void testReceiveShortPayload() {
        // Given
        uint32_t byteReqId = 12;
        uint8_t payload[] = {1, 2, 3, 4};

        BytesDTO bytes0(byteReqId, 0, true, payload, 4);
        HiveMindHostApiRequestDTO hmReq0(bytes0);
        RequestDTO request0(42, hmReq0);
        MessageDTO message0(0, 0, request0);

        m_bridge->onBytesReceived([&](uint8_t* bytes, uint64_t bytesLength) {
            g_functionCalledCount++;
            uint8_t expectedBytes[] = {1, 2, 3, 4};

            ASSERT_EQ(bytesLength, 4);
            ASSERT_EQ(memcmp(bytes, expectedBytes, 4), 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(2 * THREAD_DELAY_MS));
        });

        // When
        m_clientSerializer->serializeToStream(message0);

        // Then
        std::this_thread::sleep_for(std::chrono::milliseconds(2 * THREAD_DELAY_MS));
        ASSERT_EQ(g_functionCalledCount, 1);

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
        RequestDTO request2(42, hmReq2);
        MessageDTO message2(0, 0, request2);

        BytesDTO bytesLastMsg(byteReqId, 3, true, payload, 4);
        HiveMindHostApiRequestDTO hmReqLastMsg(bytesLastMsg);
        RequestDTO requestLastMsg(42, hmReqLastMsg);
        MessageDTO incomingLastMsg(0, 0, requestLastMsg);

        m_bridge->onBytesReceived([&](uint8_t* bytes, uint64_t bytesLength) {
            g_functionCalledCount++;
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
        std::this_thread::sleep_for(std::chrono::milliseconds(10 * THREAD_DELAY_MS));
        ASSERT_EQ(g_functionCalledCount, 1);

        cleanUpAfterTest();
    }

    void testManageTwoIntertwinedRequests() {
        // Given

        // Common stuff
        uint8_t payload[] = {1, 2, 3, 4};

        // First request
        uint32_t firstReqId = 14;

        BytesDTO firstBytes0(firstReqId, 0, false, payload, 4);
        HiveMindHostApiRequestDTO firstHmReq0(firstBytes0);
        RequestDTO firstRequest0(42, firstHmReq0);
        MessageDTO firstMessage0(0, 0, firstRequest0);

        BytesDTO firstBytes1(firstReqId, 1, false, payload, 4);
        HiveMindHostApiRequestDTO firstHmReq1(firstBytes1);
        RequestDTO firstRequest1(42, firstHmReq1);
        MessageDTO firstMessage1(0, 0, firstRequest1);

        BytesDTO firstBytes2(firstReqId, 2, true, payload, 4);
        HiveMindHostApiRequestDTO firstHmReq2(firstBytes2);
        RequestDTO firstRequest2(42, firstHmReq2);
        MessageDTO firstMessage2(0, 0, firstRequest2);

        // Second request
        uint32_t secondReqId = 15;

        BytesDTO secondBytes0(secondReqId, 0, false, payload, 4);
        HiveMindHostApiRequestDTO secondHmReq0(secondBytes0);
        RequestDTO secondRequest0(42, secondHmReq0);
        MessageDTO secondMessage0(0, 0, secondRequest0);

        BytesDTO secondBytes1(secondReqId, 1, false, payload, 4);
        HiveMindHostApiRequestDTO secondHmReq1(secondBytes1);
        RequestDTO secondRequest1(42, secondHmReq1);
        MessageDTO secondMessage1(0, 0, secondRequest1);

        BytesDTO secondBytes2(secondReqId, 2, true, payload, 4);
        HiveMindHostApiRequestDTO secondHmReq2(secondBytes2);
        RequestDTO secondRequest2(42, secondHmReq2);
        MessageDTO secondMessage2(0, 0, secondRequest2);

        m_bridge->onBytesReceived([&](uint8_t* bytes, uint64_t bytesLength) {
            g_functionCalledCount++;
            uint8_t expectedBytes[] = {
                1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4,
            };

            ASSERT_EQ(bytesLength, 12);
            ASSERT_EQ(memcmp(bytes, expectedBytes, 12), 0);
        });

        // When

        // We intertwine the messages from both requests
        m_clientSerializer->serializeToStream(firstMessage0);
        m_clientSerializer->serializeToStream(secondMessage0);

        m_clientSerializer->serializeToStream(firstMessage1);
        m_clientSerializer->serializeToStream(secondMessage1);

        m_clientSerializer->serializeToStream(firstMessage2);
        m_clientSerializer->serializeToStream(secondMessage2);

        // Then
        std::this_thread::sleep_for(std::chrono::milliseconds(20 * THREAD_DELAY_MS));
        ASSERT_EQ(g_functionCalledCount, 2);

        cleanUpAfterTest();
    }
};

TEST_F(ReceiveBytesIntegrationTestFixture, testUserCallbacks) {
    testReceiveShortPayload();
    testReceiveLongPayload();
    testManageTwoIntertwinedRequests();
}
