#include "hivemind-bridge/HiveMindHostApiRequestHandler.h"
#include "utils/Logger.h"
#include <gmock/gmock.h>

class HiveMindHostApiRequestHandlerFixture : public testing::Test {
  protected:
    Logger m_logger;
    std::unique_ptr<HiveMindHostApiRequestHandler> m_hmRequestHandler;

    // Value to test side effects
    bool m_testFunctionCalled = false;

    void SetUp() override {
        m_hmRequestHandler = std::make_unique<HiveMindHostApiRequestHandler>(m_logger);
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

    BytesDTO bytes(1, 0, false, payload, 4);
    HiveMindHostApiRequestDTO hmReq(bytes);
    RequestDTO request(42, hmReq);
    MessageDTO incomingMessage(0, 0, request);

    BytesDTO bytesLastMsg(1, 0, false, payload, 4);
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
    for (int i = 0; i < 3; i++) {
        m_hmRequestHandler->handleMessage(incomingMessage, hmReq);
    }

    m_hmRequestHandler->handleMessage(incomingLastMsg, hmReqLastMsg);

    // Then
    ASSERT_TRUE(m_testFunctionCalled);
}