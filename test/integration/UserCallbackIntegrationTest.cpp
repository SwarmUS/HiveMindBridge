#include "../utils/HiveMindBridgeFixture.h"
#include "../utils/Logger.h"
#include "../utils/TCPClient.h"
#include "hivemind-bridge/HiveMindBridge.h"
#include <atomic>
#include <gmock/gmock.h>
#include <pheromones/FunctionCallArgumentDTO.h>
#include <pheromones/FunctionCallRequestDTO.h>
#include <pheromones/HiveMindHostDeserializer.h>
#include <pheromones/HiveMindHostSerializer.h>
#include <pheromones/MessageDTO.h>
#include <pheromones/RequestDTO.h>
#include <pheromones/UserCallRequestDTO.h>
#include <thread>

// Global variables to test side effects from callbacks
int g_posX = 0;
int g_posY = 0;

class UserCallbackIntegrationTestFixture : public testing::Test, public HiveMindBridgeFixture {
  protected:
    void SetUp() override {
        std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_DELAY_MS));

        setUpCallbacks();
    }

    void TearDown() override { cleanUpAfterTest(); };

    void setUpCallbacks() {
        // Register custom actions
        CallbackFunction sideEffectCallback = [&](CallbackArgs args,
                                                  int argsLength) -> std::optional<CallbackReturn> {
            int64_t x = std::get<int64_t>(args[0].getArgument());
            int64_t y = std::get<int64_t>(args[1].getArgument());

            g_posX += x;
            g_posY += y;

            return {};
        };

        CallbackArgsManifest sideEffectManifest;
        sideEffectManifest.push_back(
            UserCallbackArgumentDescription("x", FunctionDescriptionArgumentTypeDTO::Int));
        sideEffectManifest.push_back(
            UserCallbackArgumentDescription("y", FunctionDescriptionArgumentTypeDTO::Int));
        m_bridge->registerCustomAction("sideEffect", sideEffectCallback, sideEffectManifest);

        CallbackFunction getInstantaneousPayload =
            [&](CallbackArgs args, int argsLength) -> std::optional<CallbackReturn> {
            int64_t retVal = 1;

            CallbackArgs returnArgs;
            returnArgs[0] = FunctionCallArgumentDTO(retVal);

            CallbackReturn cbReturn("getInstantaneousPayloadReturn", returnArgs);

            return cbReturn;
        };
        m_bridge->registerCustomAction("getInstantaneousPayload", getInstantaneousPayload);

        CallbackFunction getDelayedPayload = [&](CallbackArgs args,
                                                 int argsLength) -> std::optional<CallbackReturn> {
            int64_t retVal = 1;

            CallbackArgs returnArgs;
            returnArgs[0] = FunctionCallArgumentDTO(retVal);

            CallbackReturn cbReturn("getDelayedPayloadReturn", returnArgs);

            // Simulating a blocking callback
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));

            return cbReturn;
        };
        m_bridge->registerCustomAction("getDelayedPayload", getDelayedPayload);
    }

  public:
    // Teardown method that needs to be run manually since we run everything inside a single test
    // case.
    static void cleanUpAfterTest() {
        g_posX = 0;
        g_posY = 0;
    }

    void testSideEffectSuccess() {
        // Given
        FunctionCallArgumentDTO x((int64_t)1);
        FunctionCallArgumentDTO y((int64_t)1);
        FunctionCallArgumentDTO args[] = {x, y};

        FunctionCallRequestDTO functionCallRequest("sideEffect", args, 2);
        UserCallRequestDTO ucReq(UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST,
                                 functionCallRequest);
        RequestDTO req(865, ucReq);
        MessageDTO reqMsg(CLIENT_AGENT_ID, CLIENT_AGENT_ID, req);

        // When
        m_clientSerializer->serializeToStream(reqMsg);

        MessageDTO responseMessage;
        m_clientDeserializer->deserializeFromStream(responseMessage);

        // Then
        auto response = std::get<ResponseDTO>(responseMessage.getMessage());
        auto userCallResponse = std::get<UserCallResponseDTO>(response.getResponse());
        auto functionCallResponse =
            std::get<FunctionCallResponseDTO>(userCallResponse.getResponse());
        GenericResponseStatusDTO status = functionCallResponse.getResponse().getStatus();

        // Check response message
        ASSERT_EQ(response.getId(), 865);
        ASSERT_EQ(userCallResponse.getDestination(), UserCallTargetDTO::BUZZ);
        ASSERT_EQ(status, GenericResponseStatusDTO::Ok);

        // Check that the function was called (side-effects)
        ASSERT_EQ(g_posX, 1);
        ASSERT_EQ(g_posY, 1);
        cleanUpAfterTest();
    }

    void testSideEffectFail() {
        // Given
        FunctionCallArgumentDTO x((int64_t)1);
        FunctionCallArgumentDTO y((int64_t)1);
        FunctionCallArgumentDTO args[] = {x, y};

        FunctionCallRequestDTO functionCallRequest("NON_EXISTENT_FUNCTION", args, 2);
        UserCallRequestDTO ucReq(UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST,
                                 functionCallRequest);
        RequestDTO req(865, ucReq);
        MessageDTO reqMsg(CLIENT_AGENT_ID, CLIENT_AGENT_ID, req);

        // When
        m_clientSerializer->serializeToStream(reqMsg);

        MessageDTO responseMessage;
        m_clientDeserializer->deserializeFromStream(responseMessage);

        // Then
        auto response = std::get<ResponseDTO>(responseMessage.getMessage());
        auto userCallResponse = std::get<UserCallResponseDTO>(response.getResponse());
        auto functionCallResponse =
            std::get<FunctionCallResponseDTO>(userCallResponse.getResponse());
        GenericResponseStatusDTO status = functionCallResponse.getResponse().getStatus();
        std::string details = functionCallResponse.getResponse().getDetails();

        // Check response message
        ASSERT_EQ(response.getId(), 865);
        ASSERT_EQ(userCallResponse.getDestination(), UserCallTargetDTO::BUZZ);
        ASSERT_EQ(status, GenericResponseStatusDTO::BadRequest);

        // Check that there were NO side effects.
        ASSERT_EQ(g_posX, 0);
        ASSERT_EQ(g_posY, 0);
        cleanUpAfterTest();
    }

    void testGetInstantaneousPayload() {
        // Given
        FunctionCallRequestDTO functionCallRequest("getInstantaneousPayload", nullptr, 0);
        UserCallRequestDTO ucReq(UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST,
                                 functionCallRequest);
        RequestDTO req(865, ucReq);
        MessageDTO reqMsg(CLIENT_AGENT_ID, CLIENT_AGENT_ID, req);

        // When
        m_clientSerializer->serializeToStream(reqMsg);

        MessageDTO responseMessage;
        m_clientDeserializer->deserializeFromStream(responseMessage);

        // Then

        // Check response message
        auto response = std::get<ResponseDTO>(responseMessage.getMessage());
        auto userCallResponse = std::get<UserCallResponseDTO>(response.getResponse());
        auto functionCallResponse =
            std::get<FunctionCallResponseDTO>(userCallResponse.getResponse());
        GenericResponseStatusDTO status = functionCallResponse.getResponse().getStatus();
        std::string details = functionCallResponse.getResponse().getDetails();

        ASSERT_EQ(response.getId(), 865);
        ASSERT_EQ(userCallResponse.getDestination(), UserCallTargetDTO::BUZZ);
        ASSERT_EQ(status, GenericResponseStatusDTO::Ok);

        // Check return payload
        MessageDTO returnMessage;
        m_clientDeserializer->deserializeFromStream(returnMessage);

        auto returnReq = std::get<RequestDTO>(returnMessage.getMessage());
        auto returnUcReq = std::get<UserCallRequestDTO>(returnReq.getRequest());
        auto returnFunctionCallReq = std::get<FunctionCallRequestDTO>(returnUcReq.getRequest());
        std::string returnFunctionName = returnFunctionCallReq.getFunctionName();
        std::array<FunctionCallArgumentDTO, FUNCTION_ARGUMENT_COUNT> args =
            returnFunctionCallReq.getArguments();

        ASSERT_STREQ(returnFunctionName.c_str(), "getInstantaneousPayloadReturn");
        ASSERT_EQ(std::get<int64_t>(args[0].getArgument()), 1);
        cleanUpAfterTest();
    }

    void testGetDelayedPayload() {
        // Given
        FunctionCallRequestDTO functionCallRequest("getDelayedPayload", nullptr, 0);
        UserCallRequestDTO ucReq(UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST,
                                 functionCallRequest);
        RequestDTO req(865, ucReq);
        MessageDTO reqMsg(CLIENT_AGENT_ID, CLIENT_AGENT_ID, req);

        // When
        m_clientSerializer->serializeToStream(reqMsg);

        MessageDTO responseMessage;
        m_clientDeserializer->deserializeFromStream(responseMessage);

        // Then

        // Check response message
        auto response = std::get<ResponseDTO>(responseMessage.getMessage());
        auto userCallResponse = std::get<UserCallResponseDTO>(response.getResponse());
        auto functionCallResponse =
            std::get<FunctionCallResponseDTO>(userCallResponse.getResponse());
        GenericResponseStatusDTO status = functionCallResponse.getResponse().getStatus();
        std::string details = functionCallResponse.getResponse().getDetails();

        ASSERT_EQ(response.getId(), 865);
        ASSERT_EQ(userCallResponse.getDestination(), UserCallTargetDTO::BUZZ);
        ASSERT_EQ(status, GenericResponseStatusDTO::Ok);

        // Check return payload
        MessageDTO returnMessage;
        m_clientDeserializer->deserializeFromStream(
            returnMessage); // Should block until some data is available

        auto returnReq = std::get<RequestDTO>(returnMessage.getMessage());
        auto returnUcReq = std::get<UserCallRequestDTO>(returnReq.getRequest());
        auto returnFunctionCallReq = std::get<FunctionCallRequestDTO>(returnUcReq.getRequest());
        std::string returnFunctionName = returnFunctionCallReq.getFunctionName();
        std::array<FunctionCallArgumentDTO, FUNCTION_ARGUMENT_COUNT> args =
            returnFunctionCallReq.getArguments();

        ASSERT_STREQ(returnFunctionName.c_str(), "getDelayedPayloadReturn");
        ASSERT_EQ(std::get<int64_t>(args[0].getArgument()), 1);
        cleanUpAfterTest();
    }
};

TEST_F(UserCallbackIntegrationTestFixture, testUserCallbacks) {
    // sideEffect callback that edits two values
    testSideEffectSuccess();
    testSideEffectFail();

    // getInstantaneousPayload (payload return)
    testGetInstantaneousPayload();

    // getDelayedPayload
    testGetDelayedPayload();
}
