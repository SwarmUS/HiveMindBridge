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
            [&](const CallbackArgs& args) -> std::optional<CallbackReturn> { return {}; };

        CallbackArgsManifest moveByManifest;
        moveByManifest.push_back(
            UserCallbackArgumentDescription("x", FunctionDescriptionArgumentTypeDTO::Float));
        moveByManifest.push_back(
            UserCallbackArgumentDescription("y", FunctionDescriptionArgumentTypeDTO::Float));
        m_bridge->registerCustomAction("moveBy", moveByCallback, moveByManifest);

        CallbackFunction getStatus =
            [&](const CallbackArgs& args) -> std::optional<CallbackReturn> {
            int64_t isRobotOk = 1;

            CallbackArgs returnArgs;
            returnArgs.push_back(FunctionCallArgumentDTO(isRobotOk));

            CallbackReturn cbReturn("getStatusReturn", returnArgs);

            return cbReturn;
        };
        m_bridge->registerCustomAction("getStatus", getStatus);
    }

    void SetUp() override {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        setUpCallbacks();
    }
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
        FunctionDescriptionRequestDTO functionDescriptionRequest1(1);
        UserCallRequestDTO descriptionUserCallRequest1(
            UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST, functionDescriptionRequest1);
        RequestDTO descriptionRequest1(1, descriptionUserCallRequest1);
        MessageDTO descriptionRequestMessage1(1, 1, descriptionRequest1);

        // When
        m_clientSerializer->serializeToStream(descriptionRequestMessage1);

        MessageDTO descriptionResponseMessage1;
        m_clientDeserializer->deserializeFromStream(descriptionResponseMessage1);

        // Then
        auto descriptionResponse1 = std::get<ResponseDTO>(descriptionResponseMessage1.getMessage());
        auto descriptionUserCallResponse1 =
            std::get<UserCallResponseDTO>(descriptionResponse1.getResponse());
        auto functionDescriptionResponse1 =
            std::get<FunctionDescriptionResponseDTO>(descriptionUserCallResponse1.getResponse());
        auto functionDescription1 =
            std::get<FunctionDescriptionDTO>(functionDescriptionResponse1.getResponse());

        ASSERT_STREQ(functionDescription1.getFunctionName(), "getStatus");
        ASSERT_EQ(functionDescription1.getArgumentsLength(), 0);
    }
}
