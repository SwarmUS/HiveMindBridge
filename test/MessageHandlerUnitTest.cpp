#include "hivemind-bridge/MessageHandler.h"
#include "mocks/UserCallRequestManagerInterfaceMock.h"
#include "mocks/HiveMindHostApiRequestHandlerInterfaceMock.h"
#include "utils/Logger.h"
#include <gmock/gmock.h>
#include <pheromones/FunctionCallArgumentDTO.h>
#include <pheromones/MessageDTO.h>
#include <pheromones/RequestDTO.h>
#include <pheromones/UserCallRequestDTO.h>

class MessageHandlerFixture : public testing::Test {
  protected:
    Logger m_logger;
    UserCallRequestManagerInterfaceMock m_userCallRequestHandler;
    HiveMindHostApiRequestHandlerInterfaceMock m_hmRequestHandler;
    MessageHandler* m_messageHandler;

    void SetUp() override {
        m_messageHandler = new MessageHandler(m_logger, m_userCallRequestHandler, m_hmRequestHandler);
    }

    void TearDown() override { delete m_messageHandler; }
};

TEST_F(MessageHandlerFixture, handleUserCallRequest_Success) {
    // Given
    FunctionListLengthRequestDTO functionListLengthRequest;
    UserCallRequestDTO userCallRequest(UserCallTargetDTO::UNKNOWN, UserCallTargetDTO::HOST,
                                       functionListLengthRequest);
    RequestDTO request(42, userCallRequest);
    MessageDTO incomingMessage(0, 0, request);

    // When
    EXPECT_CALL(m_userCallRequestHandler, handleMessage)
        .WillOnce(testing::Return(InboundRequestHandle()));
    auto vHandle = m_messageHandler->handleMessage(incomingMessage);

    // Then
    ASSERT_TRUE(std::holds_alternative<InboundRequestHandle>(vHandle));
}

TEST_F(MessageHandlerFixture, handleInboundUserCallResponse) {
    // Given
    GenericResponseDTO genericResponse(GenericResponseStatusDTO::Ok, "All good");
    UserCallResponseDTO userCallResponse(UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST,
                                         genericResponse);
    ResponseDTO response(1, userCallResponse);
    MessageDTO incomingMessage(0, 0, response);

    // When
    InboundResponseHandle responseHandle =
        std::get<InboundResponseHandle>(m_messageHandler->handleMessage(incomingMessage));

    // Then
    ASSERT_EQ(responseHandle.getResponseId(), 1);
    ASSERT_EQ(responseHandle.getResponseStatus(), GenericResponseStatusDTO::Ok);
    ASSERT_STREQ(responseHandle.getStatusDetails().c_str(), "All good");
}

TEST_F(MessageHandlerFixture, handleInboundGenericResponse) {
    // Given
    GenericResponseDTO genericResponse(GenericResponseStatusDTO::Ok, "All good");
    ResponseDTO response(1, genericResponse);
    MessageDTO incomingMessage(0, 0, response);

    // When
    InboundResponseHandle responseHandle =
        std::get<InboundResponseHandle>(m_messageHandler->handleMessage(incomingMessage));

    // Then
    ASSERT_EQ(responseHandle.getResponseId(), 1);
    ASSERT_EQ(responseHandle.getResponseStatus(), GenericResponseStatusDTO::Ok);
    ASSERT_STREQ(responseHandle.getStatusDetails().c_str(), "All good");
}

TEST_F(MessageHandlerFixture, handleInboundFunctionResponse) {
    // Given
    GenericResponseDTO genericResponse(GenericResponseStatusDTO::Ok, "All good");
    FunctionCallResponseDTO functionCallResponse(genericResponse);
    UserCallResponseDTO userCallResponse(UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST,
                                         functionCallResponse);
    ResponseDTO response(1, userCallResponse);
    MessageDTO incomingMessage(0, 0, response);

    // When
    InboundResponseHandle responseHandle =
        std::get<InboundResponseHandle>(m_messageHandler->handleMessage(incomingMessage));

    // Then
    ASSERT_EQ(responseHandle.getResponseId(), 1);
    ASSERT_EQ(responseHandle.getResponseStatus(), GenericResponseStatusDTO::Ok);
    ASSERT_STREQ(responseHandle.getStatusDetails().c_str(), "All good");
}
