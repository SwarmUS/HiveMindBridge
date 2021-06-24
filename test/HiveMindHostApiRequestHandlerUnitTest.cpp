#include "hivemind-bridge/BytesAccumulator.h"
#include "hivemind-bridge/HiveMindHostApiRequestHandler.h"
#include "utils/Logger.h"
#include <gmock/gmock.h>

class HiveMindHostApiRequestHandlerFixture : public testing::Test {
  protected:
    Logger m_logger;
    std::unique_ptr<HiveMindHostApiRequestHandler> m_hmRequestHandler;

    // Value to test side effects
    int m_functionCalledCount = 0;

    void SetUp() override {
        m_hmRequestHandler = std::make_unique<HiveMindHostApiRequestHandler>(m_logger);
        m_functionCalledCount = 0;
    }

    void TearDown() override {}
};

TEST_F(HiveMindHostApiRequestHandlerFixture, onBytesReceived_noOverwrite) {
    // Given

    // When
    bool wasOverwritten =
        m_hmRequestHandler->onBytesReceived([](uint8_t* bytes, uint64_t bytesLength) {});

    // Then
    ASSERT_FALSE(wasOverwritten);
}

TEST_F(HiveMindHostApiRequestHandlerFixture, onBytesReceived_overwrite) {
    // Given

    // When
    m_hmRequestHandler->onBytesReceived([](uint8_t* bytes, uint64_t bytesLength) {});
    bool wasOverwritten =
        m_hmRequestHandler->onBytesReceived([](uint8_t* bytes, uint64_t bytesLength) {});

    // Then
    ASSERT_TRUE(wasOverwritten);
}

TEST_F(HiveMindHostApiRequestHandlerFixture, handleBytes_successShortBytes) {
    // Given
    uint8_t payload = 0xFF;
    BytesDTO bytes(1, 0, true, &payload, 1);
    HiveMindHostApiRequestDTO hmReq(bytes);
    RequestDTO request(42, hmReq);
    MessageDTO incomingMessage(0, 0, request);

    m_hmRequestHandler->onBytesReceived([&](uint8_t* bytes, uint64_t bytesLength) {
        m_functionCalledCount++;
        ASSERT_EQ(bytesLength, 1);
        ASSERT_EQ(bytes[0], 0xFF);
    });

    // When
    m_hmRequestHandler->handleMessage(incomingMessage, hmReq);

    // Then
    ASSERT_EQ(m_functionCalledCount, 1);
}

TEST_F(HiveMindHostApiRequestHandlerFixture, handleBytes_failNoCallback) {
    // Given
    uint8_t payload = 0xFF;
    BytesDTO bytes(1, 0, true, &payload, 1);
    HiveMindHostApiRequestDTO hmReq(bytes);
    RequestDTO request(42, hmReq);
    MessageDTO incomingMessage(0, 0, request);

    // When
    m_hmRequestHandler->handleMessage(incomingMessage, hmReq);

    // Then
    ASSERT_EQ(m_functionCalledCount, 0);
}

TEST_F(HiveMindHostApiRequestHandlerFixture, handleBytes_longPayload_Success) {
    // Given
    uint32_t byteReqId = 12;
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

    uint8_t expectedBytes[] = {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4};

    m_hmRequestHandler->onBytesReceived([&](uint8_t* bytes, uint64_t bytesLength) {
        m_functionCalledCount = 1;
        ASSERT_EQ(bytesLength, 16);
        ASSERT_EQ(memcmp(bytes, expectedBytes, 16), 0);
    });

    // When
    m_hmRequestHandler->handleMessage(message0, hmReq0);
    m_hmRequestHandler->handleMessage(message1, hmReq1);
    m_hmRequestHandler->handleMessage(message2, hmReq2);

    m_hmRequestHandler->handleMessage(incomingLastMsg, hmReqLastMsg);

    // Then
    ASSERT_EQ(m_functionCalledCount, 1);
}

TEST_F(HiveMindHostApiRequestHandlerFixture, handleBytes_intertwinedRequests_Success) {
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

    m_hmRequestHandler->onBytesReceived([&](uint8_t* bytes, uint64_t bytesLength) {
        m_functionCalledCount++;
        uint8_t expectedBytes[] = {
            1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4,
        };

        ASSERT_EQ(bytesLength, 12);
        ASSERT_EQ(memcmp(bytes, expectedBytes, 12), 0);
    });

    // When

    // We intertwine the messages from both requests
    m_hmRequestHandler->handleMessage(firstMessage0, firstHmReq0);
    m_hmRequestHandler->handleMessage(secondMessage0, secondHmReq0);

    m_hmRequestHandler->handleMessage(firstMessage1, firstHmReq1);
    m_hmRequestHandler->handleMessage(secondMessage1, secondHmReq1);

    m_hmRequestHandler->handleMessage(firstMessage2, firstHmReq2);
    m_hmRequestHandler->handleMessage(secondMessage2, secondHmReq2);

    // Then
    ASSERT_EQ(m_functionCalledCount, 2);
}