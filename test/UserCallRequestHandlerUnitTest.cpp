#include "hivemind-bridge/user-call/UserCallRequestHandler.h"
#include "mocks/UserCallbackMapInterfaceMock.h"
#include "utils/Logger.h"
#include <gmock/gmock.h>

class UserCallRequestManagerFixture : public testing::Test {
  protected:
    Logger m_logger;
    UserCallbackMapInterfaceMock m_userCallbackMap;
    std::unique_ptr<UserCallRequestHandler> m_userCallRequestHandler;

    // Value to test side effects
    bool m_testFunctionCalled = false;

    CallbackFunction m_testFunction = [&](CallbackArgs args,
                                          int argsLength) -> std::optional<CallbackReturn> {
        m_testFunctionCalled = true;

        return {};
    };

    CallbackArgsManifest m_testManifest;

    void SetUp() override {
        m_userCallRequestHandler =
            std::make_unique<UserCallRequestHandler>(m_logger, m_userCallbackMap);

        // Manifests
        m_testManifest.push_back(
            UserCallbackArgumentDescription("x", FunctionDescriptionArgumentTypeDTO::Int));
        m_testManifest.push_back(
            UserCallbackArgumentDescription("y", FunctionDescriptionArgumentTypeDTO::Float));
    }

    void TearDown() override {}
};

TEST_F(UserCallRequestManagerFixture, testHandlefunctionDescriptionRequest_Success) {
    // Given
    FunctionDescriptionRequestDTO functionDescriptionRequest(0);
    UserCallRequestDTO userCallRequest(UserCallTargetDTO::UNKNOWN, UserCallTargetDTO::HOST,
                                       functionDescriptionRequest);
    RequestDTO request(42, userCallRequest);
    MessageDTO incomingMessage(0, 0, request);

    // When
    EXPECT_CALL(m_userCallbackMap, getManifestAt(testing::_))
        .WillOnce(testing::Return(m_testManifest));
    EXPECT_CALL(m_userCallbackMap, getNameAt(testing::_)).WillOnce(testing::Return("moveBy"));

    InboundRequestHandle result = std::get<InboundRequestHandle>(
        m_userCallRequestHandler->handleMessage(incomingMessage, userCallRequest));

    // Then
    MessageDTO responseMessage = result.getResponse();
    ResponseDTO response = std::get<ResponseDTO>(responseMessage.getMessage());
    UserCallResponseDTO userCallResponse = std::get<UserCallResponseDTO>(response.getResponse());
    FunctionDescriptionResponseDTO functionDescriptionResponse =
        std::get<FunctionDescriptionResponseDTO>(userCallResponse.getResponse());
    FunctionDescriptionDTO functionDescription =
        std::get<FunctionDescriptionDTO>(functionDescriptionResponse.getResponse());

    ASSERT_EQ(functionDescription.getArgumentsLength(), 2);
    ASSERT_STREQ(functionDescription.getFunctionName(), "moveBy");

    auto args = functionDescription.getArguments();
    ASSERT_STREQ(args[0].getArgumentName(), "x");
    ASSERT_EQ(args[0].getArgumentType(), FunctionDescriptionArgumentTypeDTO::Int);

    ASSERT_STREQ(args[1].getArgumentName(), "y");
    ASSERT_EQ(args[1].getArgumentType(), FunctionDescriptionArgumentTypeDTO::Float);
}

TEST_F(UserCallRequestManagerFixture, testHandlefunctionDescriptionRequestNoInputArgs_Success) {
    CallbackArgsManifest noArgs;

    // Given
    FunctionDescriptionRequestDTO functionDescriptionRequest(1);
    UserCallRequestDTO userCallRequest(UserCallTargetDTO::UNKNOWN, UserCallTargetDTO::HOST,
                                       functionDescriptionRequest);
    RequestDTO request(42, userCallRequest);
    MessageDTO incomingMessage(0, 0, request);

    // When
    EXPECT_CALL(m_userCallbackMap, getManifestAt(testing::_)).WillOnce(testing::Return(noArgs));
    EXPECT_CALL(m_userCallbackMap, getNameAt(testing::_))
        .WillOnce(testing::Return("noInputArgsFunction"));
    InboundRequestHandle result = std::get<InboundRequestHandle>(
        m_userCallRequestHandler->handleMessage(incomingMessage, userCallRequest));

    // Then
    MessageDTO responseMessage = result.getResponse();
    ResponseDTO response = std::get<ResponseDTO>(responseMessage.getMessage());
    UserCallResponseDTO userCallResponse = std::get<UserCallResponseDTO>(response.getResponse());
    FunctionDescriptionResponseDTO functionDescriptionResponse =
        std::get<FunctionDescriptionResponseDTO>(userCallResponse.getResponse());
    FunctionDescriptionDTO functionDescription =
        std::get<FunctionDescriptionDTO>(functionDescriptionResponse.getResponse());

    ASSERT_EQ(functionDescription.getArgumentsLength(), 0);
    ASSERT_STREQ(functionDescription.getFunctionName(), "noInputArgsFunction");
}

TEST_F(UserCallRequestManagerFixture, testHandlefunctionDescriptionRequest_OutOfBounds) {
    // Given
    FunctionDescriptionRequestDTO functionDescriptionRequest(99); // out of bounds!
    UserCallRequestDTO userCallRequest(UserCallTargetDTO::UNKNOWN, UserCallTargetDTO::HOST,
                                       functionDescriptionRequest);
    RequestDTO request(42, userCallRequest);
    MessageDTO incomingMessage(0, 0, request);

    // When
    EXPECT_CALL(m_userCallbackMap, getManifestAt(testing::_))
        .WillOnce(testing::Return(std::nullopt));
    InboundRequestHandle result = std::get<InboundRequestHandle>(
        m_userCallRequestHandler->handleMessage(incomingMessage, userCallRequest));

    // Then
    MessageDTO responseMessage = result.getResponse();
    ResponseDTO response = std::get<ResponseDTO>(responseMessage.getMessage());
    UserCallResponseDTO userCallResponse = std::get<UserCallResponseDTO>(response.getResponse());

    FunctionDescriptionResponseDTO fdResponse =
        std::get<FunctionDescriptionResponseDTO>(userCallResponse.getResponse());
    GenericResponseDTO genericResponse = std::get<GenericResponseDTO>(fdResponse.getResponse());
    ASSERT_EQ(genericResponse.getStatus(), GenericResponseStatusDTO::BadRequest);
    ASSERT_STREQ(genericResponse.getDetails(), "Index out of bounds.");
}

TEST_F(UserCallRequestManagerFixture, testHandlefunctionListLengthRequest_Success) {
    // Given
    FunctionListLengthRequestDTO functionListLengthRequest;
    UserCallRequestDTO userCallRequest(UserCallTargetDTO::UNKNOWN, UserCallTargetDTO::HOST,
                                       functionListLengthRequest);
    RequestDTO request(42, userCallRequest);
    MessageDTO incomingMessage(0, 0, request);

    // When
    EXPECT_CALL(m_userCallbackMap, getLength()).WillOnce(testing::Return(6));
    InboundRequestHandle result = std::get<InboundRequestHandle>(
        m_userCallRequestHandler->handleMessage(incomingMessage, userCallRequest));

    // Then
    MessageDTO responseMessage = result.getResponse();
    ResponseDTO response = std::get<ResponseDTO>(responseMessage.getMessage());
    UserCallResponseDTO userCallResponse = std::get<UserCallResponseDTO>(response.getResponse());
    FunctionListLengthResponseDTO functionListLengthResponse =
        std::get<FunctionListLengthResponseDTO>(userCallResponse.getResponse());

    ASSERT_EQ(functionListLengthResponse.getLength(), 6);
}

TEST_F(UserCallRequestManagerFixture, handleFunctionCall_Success) {
    // Given
    FunctionCallRequestDTO fcReq("TestFunctionCallRequestDTO", nullptr, 0);
    UserCallRequestDTO ucReq(UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST, fcReq);
    RequestDTO req(1, ucReq);
    MessageDTO incomingMessage(99, 2, req);

    // When
    EXPECT_CALL(m_userCallbackMap, getCallback(testing::_))
        .WillOnce(testing::Return(m_testFunction));
    InboundRequestHandle result = std::get<InboundRequestHandle>(
        m_userCallRequestHandler->handleMessage(incomingMessage, ucReq));

    // Then
    MessageDTO responseMessage = result.getResponse();

    ASSERT_EQ(responseMessage.getSourceId(), 2);
    ASSERT_EQ(responseMessage.getDestinationId(), 99);

    ResponseDTO response = std::get<ResponseDTO>(responseMessage.getMessage());
    ASSERT_EQ(response.getId(), 1);

    UserCallResponseDTO userCallResponse = std::get<UserCallResponseDTO>(response.getResponse());
    ASSERT_EQ(userCallResponse.getDestination(), UserCallTargetDTO::BUZZ);

    FunctionCallResponseDTO functionCallResponse =
        std::get<FunctionCallResponseDTO>(userCallResponse.getResponse());
    GenericResponseDTO genericResponse = functionCallResponse.getResponse();
    ASSERT_EQ(genericResponse.getStatus(), GenericResponseStatusDTO::Ok);

    result.getCallbackReturnContext().wait();

    ASSERT_TRUE(m_testFunctionCalled);
}

TEST_F(UserCallRequestManagerFixture, handleFunctionCall_Fail) {
    FunctionCallRequestDTO fcReq("TestFunctionCallRequestDTO", nullptr, 0);
    UserCallRequestDTO ucReq(UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST, fcReq);
    RequestDTO req(1, ucReq);
    MessageDTO incomingMessage(99, 2, req);

    // When
    EXPECT_CALL(m_userCallbackMap, getCallback(testing::_)).WillOnce(testing::Return(std::nullopt));
    InboundRequestHandle result = std::get<InboundRequestHandle>(
        m_userCallRequestHandler->handleMessage(incomingMessage, ucReq));

    // Then
    MessageDTO responseMessage = result.getResponse();

    ASSERT_EQ(responseMessage.getSourceId(), 2);
    ASSERT_EQ(responseMessage.getDestinationId(), 99);

    ResponseDTO response = std::get<ResponseDTO>(responseMessage.getMessage());
    ASSERT_EQ(response.getId(), 1);

    UserCallResponseDTO userCallResponse = std::get<UserCallResponseDTO>(response.getResponse());
    ASSERT_EQ(userCallResponse.getDestination(), UserCallTargetDTO::BUZZ);

    FunctionCallResponseDTO functionCallResponse =
        std::get<FunctionCallResponseDTO>(userCallResponse.getResponse());
    GenericResponseDTO genericResponse = functionCallResponse.getResponse();
    ASSERT_EQ(genericResponse.getStatus(), GenericResponseStatusDTO::BadRequest);
}