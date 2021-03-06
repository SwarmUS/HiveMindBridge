#include "hive_mind_bridge/MessageHandler.h"

#include <functional>
#include <gmock/gmock.h>
#include <hivemind-host/FunctionCallArgumentDTO.h>
#include <hivemind-host/FunctionCallRequestDTO.h>
#include <hivemind-host/MessageDTO.h>
#include <hivemind-host/RequestDTO.h>
#include <hivemind-host/UserCallRequestDTO.h>
#include <optional>

class MessageHandlerFixture : public testing::Test {
  protected:
    bool m_testFunctionCalled = false;
    int m_testValue1 = 0;
    int m_testValue2 = 12;
    uint32_t m_testcompoundSourceId = 0;
    uint32_t m_testCompoundDestinationId = 0;
    UserCallTargetDTO m_testModuleDestinationId = UserCallTargetDTO::UNKNOWN;
    uint32_t m_testExpectedResponseId = 0;

    // Declare some test callbacks
    CallbackFunction m_testFunction = [&](CallbackArgs args, int argsLength) {
        m_testFunctionCalled = true;
    };

    CallbackFunction m_moveByTestCallback = [&](CallbackArgs args, int argsLength) {
        m_testValue1 += std::get<int64_t>(args[0].getArgument());
        m_testValue2 -= std::get<float>(args[1].getArgument());
    };

    MessageHandler m_messageHandler;

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

        m_messageHandler.registerCallback("MoveBy", m_moveByTestCallback);

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
    }

    void TearDown() override {
        delete m_functionCallRequestDto;
        delete m_userCallRequestDto;
        delete m_requestDto;
        delete m_messageDto;

        delete m_nonExistingFunctionCallRequestDto;
        delete m_nonExistingUserCallRequestDto;
        delete m_nonExistingRequestDto;
        delete m_nonExistingMessageDto;

        delete m_sideEffectArg1;
        delete m_sideEffectArg2;
        delete m_sideEffectFunctionCallRequestDto;
        delete m_sideEffectUserCallRequestDto;
        delete m_sideEffectRequestDto;
        delete m_moveByMessageDto;
    }
};

TEST_F(MessageHandlerFixture, registerNewCallBackSuccess) {
    ASSERT_FALSE(m_messageHandler.registerCallback("TestFunctionCallRequestDTO", m_testFunction));
}

TEST_F(MessageHandlerFixture, registerOverwriteCallbackSuccess) {
    m_messageHandler.registerCallback("TestFunctionCallRequestDTO", m_testFunction);
    ASSERT_TRUE(m_messageHandler.registerCallback("TestFunctionCallRequestDTO", m_testFunction));
}

TEST_F(MessageHandlerFixture, testGetCallbackSuccess) {
    m_messageHandler.registerCallback("TestFunctionCallRequestDTO", m_testFunction);
    ASSERT_TRUE(m_messageHandler.getCallback("TestFunctionCallRequestDTO"));
}

TEST_F(MessageHandlerFixture, testGetCallbackFail) {
    ASSERT_FALSE(m_messageHandler.getCallback("Nonexisting"));
}

TEST_F(MessageHandlerFixture, testHandleMessageVoidFunctionSuccess) {
    m_messageHandler.registerCallback("TestFunctionCallRequestDTO", m_testFunction);
    MessageDTO responseMessage = m_messageHandler.handleMessage(*m_messageDto);
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

    ASSERT_TRUE(m_testFunctionCalled);
}

TEST_F(MessageHandlerFixture, TestHandleMessageMoveByFunctionSuccess) {
    MessageDTO responseMessage = m_messageHandler.handleMessage(*m_moveByMessageDto);
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

    ASSERT_EQ(m_testValue1, 1);
    ASSERT_EQ(m_testValue2, 7);
}

TEST_F(MessageHandlerFixture, testHandleMessageFail) {
    MessageDTO responseMessage = m_messageHandler.handleMessage(*m_nonExistingMessageDto);
    ASSERT_EQ(responseMessage.getSourceId(), 2);
    ASSERT_EQ(responseMessage.getDestinationId(), 99);

    ResponseDTO response = std::get<ResponseDTO>(responseMessage.getMessage());
    ASSERT_EQ(response.getId(), 1);

    UserCallResponseDTO userCallResponse = std::get<UserCallResponseDTO>(response.getResponse());
    ASSERT_EQ(userCallResponse.getDestination(), UserCallTargetDTO::BUZZ);

    FunctionCallResponseDTO functionCallResponse =
        std::get<FunctionCallResponseDTO>(userCallResponse.getResponse());
    GenericResponseDTO genericResponse = functionCallResponse.getResponse();
    ASSERT_EQ(genericResponse.getStatus(), GenericResponseStatusDTO::Unknown);
}