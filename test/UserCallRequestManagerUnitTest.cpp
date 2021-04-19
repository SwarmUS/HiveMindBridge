#include "hivemind-bridge/user-call/UserCallRequestManager.h"
#include "mocks/UserCallbackMapInterfaceMock.h"
#include "utils/Logger.h"
#include <gmock/gmock.h>

class UserCallRequestManagerFixture : public testing::Test {
  protected:
    Logger m_logger;
    UserCallbackMapInterfaceMock m_userCallbackMap;
    std::unique_ptr<UserCallRequestManager> m_userCallRequestManager;

    // Declare some test manifests
    CallbackArgsManifest m_moveByTestCallbackManifest;

    // Declare some test messages
    FunctionCallRequestDTO* m_functionCallRequestDto;
    UserCallRequestDTO* m_userCallRequestDto;
    RequestDTO* m_requestDto;
    MessageDTO* m_messageDto;

    FunctionCallRequestDTO* m_nonExistingFunctionCallRequestDto;
    UserCallRequestDTO* m_nonExistingUserCallRequestDto;
    RequestDTO* m_nonExistingRequestDto;
    MessageDTO* m_nonExistingMessageDto;

    FunctionCallArgumentDTO* m_sideEffectArg1;
    FunctionCallArgumentDTO* m_sideEffectArg2;
    FunctionCallRequestDTO* m_sideEffectFunctionCallRequestDto;
    UserCallRequestDTO* m_sideEffectUserCallRequestDto;
    RequestDTO* m_sideEffectRequestDto;

    MessageDTO* m_moveByMessageDto;

    void SetUp() override {
        m_userCallRequestManager =
            std::make_unique<UserCallRequestManager>(m_logger, m_userCallbackMap);

        // Existing void  function
        m_functionCallRequestDto =
            new FunctionCallRequestDTO("TestFunctionCallRequestDTO", nullptr, 0);
        m_userCallRequestDto = new UserCallRequestDTO(
            UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST, *m_functionCallRequestDto);
        m_requestDto = new RequestDTO(1, *m_userCallRequestDto);
        m_messageDto = new MessageDTO(99, 2, *m_requestDto);

        // Nonexisting void function
        m_nonExistingFunctionCallRequestDto = new FunctionCallRequestDTO("NonExisting", nullptr, 0);
        m_nonExistingUserCallRequestDto = new UserCallRequestDTO(
            UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST, *m_nonExistingFunctionCallRequestDto);
        m_nonExistingRequestDto = new RequestDTO(1, *m_nonExistingUserCallRequestDto);
        m_nonExistingMessageDto = new MessageDTO(99, 2, *m_nonExistingRequestDto);

        // Testing a callback with side effects
        m_sideEffectArg1 = new FunctionCallArgumentDTO((int64_t)1);
        m_sideEffectArg2 = new FunctionCallArgumentDTO((float)5);
        FunctionCallArgumentDTO args[2] = {*m_sideEffectArg1, *m_sideEffectArg2};
        m_sideEffectFunctionCallRequestDto = new FunctionCallRequestDTO("MoveBy", args, 2);
        m_sideEffectUserCallRequestDto = new UserCallRequestDTO(
            UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST, *m_sideEffectFunctionCallRequestDto);
        m_sideEffectRequestDto = new RequestDTO(1, *m_sideEffectUserCallRequestDto);
        m_moveByMessageDto = new MessageDTO(99, 2, *m_sideEffectRequestDto);

        // Manifests
        m_moveByTestCallbackManifest.push_back(
            UserCallbackArgumentDescription("x", FunctionDescriptionArgumentTypeDTO::Int));
        m_moveByTestCallbackManifest.push_back(
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
        .WillOnce(testing::Return(m_moveByTestCallbackManifest));
    EXPECT_CALL(m_userCallbackMap, getNameAt(testing::_)).WillOnce(testing::Return("moveBy"));

    InboundRequestHandle result = std::get<InboundRequestHandle>(
        m_userCallRequestManager->handleMessage(incomingMessage, userCallRequest));

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
        m_userCallRequestManager->handleMessage(incomingMessage, userCallRequest));

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
        m_userCallRequestManager->handleMessage(incomingMessage, userCallRequest));

    // Then
    MessageDTO responseMessage = result.getResponse();
    ResponseDTO response = std::get<ResponseDTO>(responseMessage.getMessage());
    UserCallResponseDTO userCallResponse = std::get<UserCallResponseDTO>(response.getResponse());

    FunctionCallResponseDTO functionCallResponse =
        std::get<FunctionCallResponseDTO>(userCallResponse.getResponse());
    GenericResponseDTO genericResponse = functionCallResponse.getResponse();
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
        m_userCallRequestManager->handleMessage(incomingMessage, userCallRequest));

    // Then
    MessageDTO responseMessage = result.getResponse();
    ResponseDTO response = std::get<ResponseDTO>(responseMessage.getMessage());
    UserCallResponseDTO userCallResponse = std::get<UserCallResponseDTO>(response.getResponse());
    FunctionListLengthResponseDTO functionListLengthResponse =
        std::get<FunctionListLengthResponseDTO>(userCallResponse.getResponse());

    ASSERT_EQ(functionListLengthResponse.getLength(), 6);
}

// TODO test function calls