#include "hivemind-bridge/HiveMindHostApiRequestHandler.h"
#include "utils/Logger.h"
#include <gmock/gmock.h>

class HiveMindHostApiRequestHandlerFixture : public testing::Test {
protected:
    Logger m_logger;
    std::unique_ptr<HiveMindHostApiRequestHandler> m_hmRequestHandler;

    void SetUp() override {
        m_hmRequestHandler = std::make_unique<HiveMindHostApiRequestHandler>(m_logger);
    }

    void TearDown() override {}
};

TEST_F(HiveMindHostApiRequestHandlerFixture, onBytesReceived_noOverwrite) {
    // Given

    // When
    bool wasOverwritten = m_hmRequestHandler->onBytesReceived([](){ });

    // Then
    ASSERT_FALSE(wasOverwritten);
}

TEST_F(HiveMindHostApiRequestHandlerFixture, onBytesReceived_overwrite) {
    // Given

    // When
    m_hmRequestHandler->onBytesReceived([](){ });
    bool wasOverwritten = m_hmRequestHandler->onBytesReceived([](){ });

    // Then
    ASSERT_TRUE(wasOverwritten);
}

TEST_F(HiveMindHostApiRequestHandlerFixture, handleMessage_successShortMessage) {
    // Given
    uint8_t payload = 0xFFFF;
    BytesDTO bytes(1, 0, true, &payload, 1);
    HiveMindHostApiRequestDTO hmReq(bytes);
    RequestDTO request(42, hmReq);
    MessageDTO incomingMessage(0, 0, request);



    // When

    // Then
}