#include "../utils/HiveMindBridgeFixture.h"
#include "../utils/Logger.h"
#include "../utils/TCPClient.h"
#include "hivemind-bridge/HiveMindBridge.h"
#include <gmock/gmock.h>
#include <pheromones/FunctionListLengthRequestDTO.h>
#include <pheromones/HiveMindHostDeserializer.h>
#include <pheromones/HiveMindHostSerializer.h>
#include <pheromones/MessageDTO.h>
#include <pheromones/RequestDTO.h>
#include <pheromones/UserCallRequestDTO.h>
#include <thread>

class FunctionDescriptionRequestIntegrationTestFixture : public testing::Test,
                                                         public HiveMindBridgeFixture {
  protected:
    void setUpCallbacks() {
        // Register custom actions
        CallbackFunction moveByCallback =
            [&](CallbackArgs args, int argsLength) -> std::optional<CallbackReturn> { return {}; };

        CallbackArgsManifest moveByManifest;
        moveByManifest.push_back(
            UserCallbackArgumentDescription("x", FunctionDescriptionArgumentTypeDTO::Float));
        moveByManifest.push_back(
            UserCallbackArgumentDescription("y", FunctionDescriptionArgumentTypeDTO::Float));
        m_bridge->registerCustomAction("moveBy", moveByCallback, moveByManifest);

        CallbackFunction getStatus = [&](CallbackArgs args,
                                         int argsLength) -> std::optional<CallbackReturn> {
            int64_t isRobotOk = 1;

            CallbackArgs returnArgs;
            returnArgs[0] = FunctionCallArgumentDTO(isRobotOk);

            CallbackReturn cbReturn("getStatusReturn", returnArgs);

            return cbReturn;
        };
        m_bridge->registerCustomAction("getStatus", getStatus);
    }

    void SetUp() {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        setUpCallbacks();
    }

    void TearDown(){};
};

TEST_F(FunctionDescriptionRequestIntegrationTestFixture, testFunctionListLengthRequest) {
    for (int i = 0; i < 5; i++) {
        // LIST LENGTH
        // Given
        FunctionListLengthRequestDTO listLengthRequest;
        UserCallRequestDTO userCallRequest(UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST,
                                           listLengthRequest);
        RequestDTO request(1, userCallRequest);
        MessageDTO requestMessage(1, 1, request);

        // When
        m_clientSerializer->serializeToStream(requestMessage);

        MessageDTO responseMessage;
        m_clientDeserializer->deserializeFromStream(responseMessage);

        // Then
        auto response = std::get<ResponseDTO>(responseMessage.getMessage());
        auto userCallResponse = std::get<UserCallResponseDTO>(response.getResponse());
        auto functionListLengthResponse =
            std::get<FunctionListLengthResponseDTO>(userCallResponse.getResponse());

        ASSERT_EQ(functionListLengthResponse.getLength(), 2);

        // FUNCTION DESCRIPTION 0

        // Given
        FunctionDescriptionRequestDTO functionDescriptionRequest(0);
        UserCallRequestDTO descriptionUserCallRequest(
            UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST, functionDescriptionRequest);
        RequestDTO descriptionRequest(1, descriptionUserCallRequest);
        MessageDTO descriptionRequestMessage(1, 1, descriptionRequest);

        // When
        m_clientSerializer->serializeToStream(descriptionRequestMessage);

        MessageDTO descriptionResponseMessage;
        m_clientDeserializer->deserializeFromStream(descriptionResponseMessage);

        // Then
        auto descriptionResponse = std::get<ResponseDTO>(descriptionResponseMessage.getMessage());
        auto descriptionUserCallResponse =
            std::get<UserCallResponseDTO>(descriptionResponse.getResponse());
        auto functionDescriptionResponse =
            std::get<FunctionDescriptionResponseDTO>(descriptionUserCallResponse.getResponse());
        auto functionDescription =
            std::get<FunctionDescriptionDTO>(functionDescriptionResponse.getResponse());

        ASSERT_STREQ(functionDescription.getFunctionName(), "moveBy");
        ASSERT_EQ(functionDescription.getArgumentsLength(), 2);

        // FUNCTION DESCRIPTION 1
        // Given
        FunctionDescriptionRequestDTO functionDescriptionRequest_1(1);
        UserCallRequestDTO descriptionUserCallRequest_1(
            UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST, functionDescriptionRequest_1);
        RequestDTO descriptionRequest_1(1, descriptionUserCallRequest_1);
        MessageDTO descriptionRequestMessage_1(1, 1, descriptionRequest_1);

        // When
        m_clientSerializer->serializeToStream(descriptionRequestMessage_1);

        MessageDTO descriptionResponseMessage_1;
        m_clientDeserializer->deserializeFromStream(descriptionResponseMessage_1);

        // Then
        auto descriptionResponse_1 =
            std::get<ResponseDTO>(descriptionResponseMessage_1.getMessage());
        auto descriptionUserCallResponse_1 =
            std::get<UserCallResponseDTO>(descriptionResponse_1.getResponse());
        auto functionDescriptionResponse_1 =
            std::get<FunctionDescriptionResponseDTO>(descriptionUserCallResponse_1.getResponse());
        auto functionDescription_1 =
            std::get<FunctionDescriptionDTO>(functionDescriptionResponse_1.getResponse());

        ASSERT_STREQ(functionDescription_1.getFunctionName(), "getStatus");
        ASSERT_EQ(functionDescription_1.getArgumentsLength(), 0);
    }
}
