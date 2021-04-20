#include "hivemind-bridge/Callback.h"
#include "hivemind-bridge/MessageHandler.h"
#include "hivemind-bridge/user-call/UserCallRequestManager.h"
#include "hivemind-bridge/user-call/UserCallbackMap.h"
#include "utils/Logger.h"
#include <gmock/gmock.h>
#include <pheromones/FunctionCallArgumentDTO.h>
#include <pheromones/FunctionCallRequestDTO.h>
#include <pheromones/FunctionDescriptionArgumentTypeDTO.h>
#include <pheromones/FunctionDescriptionRequestDTO.h>
#include <pheromones/MessageDTO.h>
#include <pheromones/RequestDTO.h>
#include <pheromones/UserCallRequestDTO.h>

class MessageHandlerFixture : public testing::Test {
  protected:
    bool m_testFunctionCalled = false;
    int m_testValue1 = 0;
    int m_testValue2 = 12;

    // Declare some test callbacks
    CallbackFunction m_testFunction = [&](CallbackArgs args,
                                          int argsLength) -> std::optional<CallbackReturn> {
        m_testFunctionCalled = true;

        return {};
    };

    CallbackFunction m_moveByTestCallback = [&](CallbackArgs args,
                                                int argsLength) -> std::optional<CallbackReturn> {
        m_testValue1 += std::get<int64_t>(args[0].getArgument());
        m_testValue2 -= std::get<float>(args[1].getArgument());

        return {};
    };
    CallbackArgsManifest m_moveByTestCallbackManifest;

    Logger m_logger;
    UserCallbackMap m_userCallbackMap;
    UserCallRequestManager* m_userCallRequestManager;
    MessageHandler* m_messageHandler;

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
        m_userCallRequestManager = new UserCallRequestManager(m_logger, m_userCallbackMap);
        m_messageHandler = new MessageHandler(m_logger, *m_userCallRequestManager);
        m_moveByTestCallbackManifest.push_back(
            UserCallbackArgumentDescription("x", FunctionDescriptionArgumentTypeDTO::Int));
        m_moveByTestCallbackManifest.push_back(
            UserCallbackArgumentDescription("y", FunctionDescriptionArgumentTypeDTO::Float));
        m_messageHandler->registerCallback("MoveBy", m_moveByTestCallback,
                                           m_moveByTestCallbackManifest);

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

        delete m_messageHandler;
        delete m_userCallRequestManager;
    }
};

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
