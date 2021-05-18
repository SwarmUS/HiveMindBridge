#include "hivemind-bridge/HiveMindHostApiRequestHandler.h"
#include "hivemind-bridge/BytesAccumulator.h"
#include "utils/Logger.h"
#include <gmock/gmock.h>

class HiveMindHostApiRequestHandlerFixture : public testing::Test {
  protected:
    Logger m_logger;
    BytesAccumulator m_accumulator;
    std::unique_ptr<HiveMindHostApiRequestHandler> m_hmRequestHandler;

    // Value to test side effects
    bool m_testFunctionCalled = false;

    void SetUp() override {
        m_hmRequestHandler = std::make_unique<HiveMindHostApiRequestHandler>(m_logger, m_accumulator);
        m_testFunctionCalled = false;
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
        m_testFunctionCalled = true;
        ASSERT_EQ(bytesLength, 1);
        ASSERT_EQ(bytes[0], 0xFF);
    });

    // When
    m_hmRequestHandler->handleMessage(incomingMessage, hmReq);

    // Then
    ASSERT_TRUE(m_testFunctionCalled);
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
    ASSERT_FALSE(m_testFunctionCalled);
}

TEST_F(HiveMindHostApiRequestHandlerFixture, handleBytes_successLongBytes_oneRequestAtATime) {
    // Given
    uint32_t byteReqId = 12;
    uint8_t payload[] = {1, 2, 3, 4};

    BytesDTO bytes0(1, 0, false, payload, 4);
    HiveMindHostApiRequestDTO hmReq0(bytes0);
    RequestDTO request0(42, hmReq0);
    MessageDTO message0(0, 0, request0);

    BytesDTO bytes1(1, 1, false, payload, 4);
    HiveMindHostApiRequestDTO hmReq1(bytes1);
    RequestDTO request1(42, hmReq1);
    MessageDTO message1(0, 0, request1);

    BytesDTO bytes2(1, 2, false, payload, 4);
    HiveMindHostApiRequestDTO hmReq2(bytes2);
    RequestDTO request2(42, hmReq0);
    MessageDTO message2(0, 0, request0);

    BytesDTO bytesLastMsg(1, 3, true, payload, 4);
    HiveMindHostApiRequestDTO hmReqLastMsg(bytesLastMsg);
    RequestDTO requestLastMsg(42, hmReqLastMsg);
    MessageDTO incomingLastMsg(0, 0, requestLastMsg);

    uint8_t expectedBytes[] = {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4};

    m_hmRequestHandler->onBytesReceived([&](uint8_t* bytes, uint64_t bytesLength) {
        m_testFunctionCalled = true;
        ASSERT_EQ(bytesLength, 16);
        ASSERT_EQ(memcmp(bytes, expectedBytes, 16), 0);
    });

    // When
    m_hmRequestHandler->handleMessage(message0, hmReq0);
    m_hmRequestHandler->handleMessage(message1, hmReq1);
    m_hmRequestHandler->handleMessage(message2, hmReq2);

    m_hmRequestHandler->handleMessage(incomingLastMsg, hmReqLastMsg);

    // Then
    ASSERT_TRUE(m_testFunctionCalled);
}