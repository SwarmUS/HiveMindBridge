#include "../utils/Logger.h"
#include "../utils/TCPClient.h"
#include "HiveMindBridge/HiveMindBridge.h"
#include <gmock/gmock.h>
#include <pheromones/HiveMindHostDeserializer.h>
#include <pheromones/HiveMindHostSerializer.h>
#include <pheromones/MessageDTO.h>
#include <pheromones/RequestDTO.h>
#include <pheromones/UserCallRequestDTO.h>
#include <pheromones/FunctionCallArgumentDTO.h>
#include <pheromones/FunctionCallRequestDTO.h>
#include <thread>
#include <atomic>

std::atomic_bool g_threadShouldRun = true;

constexpr uint32_t CLIENT_AGENT_ID = 12;
constexpr int THREAD_DELAY_MS = 100;

int g_posX = 0;
int g_posY = 0;

class UserCallbackIntegrationTestFixture : public testing::Test {
protected:
    Logger m_logger;
    int m_tcpPort = 5001;

// Bridge side
    HiveMindBridge* m_bridge;

// Client side
    TCPClient* m_tcpClient;
    HiveMindHostSerializer* m_clientSerializer;
    HiveMindHostDeserializer* m_clientDeserializer;

    void setUpCallbacks() {
        // Register custom actions
        CallbackFunction sideEffectCallback =
                [&](CallbackArgs args, int argsLength) -> std::optional<CallbackReturn> {
            int64_t x = std::get<int64_t>(args[0].getArgument());
            int64_t y = std::get<int64_t>(args[1].getArgument());

            g_posX+= x;
            g_posY+= y;

            return {};
        };

        CallbackArgsManifest sideEffectManifest;
        sideEffectManifest.push_back(
                UserCallbackArgumentDescription("x", FunctionDescriptionArgumentTypeDTO::Int));
        sideEffectManifest.push_back(
                UserCallbackArgumentDescription("y", FunctionDescriptionArgumentTypeDTO::Int));
        m_bridge->registerCustomAction("sideEffect", sideEffectCallback, sideEffectManifest);

        CallbackFunction getInstantaneousPayload = [&](CallbackArgs args,
                                         int argsLength) -> std::optional<CallbackReturn> {
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

    void connectClient() {
        std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_DELAY_MS));
        m_tcpClient->connect();
    }

    void greet() {
        // Wait for a greet message
        MessageDTO greetRequest;
        if (m_clientDeserializer->deserializeFromStream(greetRequest)) {
            MessageDTO greetResponse(CLIENT_AGENT_ID, CLIENT_AGENT_ID, GreetingDTO(CLIENT_AGENT_ID));
            m_clientSerializer->serializeToStream(greetResponse);
        } else {
            m_logger.log(LogLevel::Warn, "Deserializing greet failed.");
        }
    }

    void SetUp() { std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_DELAY_MS)); }

    void TearDown(){
        cleanUpAfterTest();
    };

public:
    UserCallbackIntegrationTestFixture() {
        // Bridge side
        m_bridge = new HiveMindBridge(m_tcpPort, m_logger);
        m_bridgeThread =
                std::thread(&UserCallbackIntegrationTestFixture::bridgeThread, this);

        setUpCallbacks();

        // Client side
        m_tcpClient = new TCPClient(m_tcpPort);
        m_clientSerializer = new HiveMindHostSerializer(*m_tcpClient);
        m_clientDeserializer = new HiveMindHostDeserializer(*m_tcpClient);

        connectClient();

        greet();
    }

    ~UserCallbackIntegrationTestFixture() {
        g_threadShouldRun = false;
        m_bridgeThread.join();
        m_tcpClient->close();

        delete m_tcpClient;
        delete m_clientSerializer;
        delete m_clientDeserializer;
        delete m_bridge;
    }

    std::thread m_bridgeThread;

    void bridgeThread() {
        while (g_threadShouldRun) {
            m_bridge->spin();
            std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_DELAY_MS));
        }
    }

    // Teardown method that needs to be run manually since we run everything inside a single test case.
    void cleanUpAfterTest() {
        g_posX = 0;
        g_posY = 0;
    }

    void testSideEffectSuccess() {
        // Given
        FunctionCallArgumentDTO x((int64_t ) 1);
        FunctionCallArgumentDTO y((int64_t ) 1);
        FunctionCallArgumentDTO args[] = {x, y};

        FunctionCallRequestDTO functionCallRequest("sideEffect", args, 2);
        UserCallRequestDTO ucReq(UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST, functionCallRequest);
        RequestDTO req(865, ucReq);
        MessageDTO reqMsg(CLIENT_AGENT_ID, CLIENT_AGENT_ID, req);

        // When
        m_clientSerializer->serializeToStream(reqMsg);

        MessageDTO responseMessage;
        m_clientDeserializer->deserializeFromStream(responseMessage);

        // Then
        auto response = std::get<ResponseDTO>(responseMessage.getMessage());
        auto userCallResponse = std::get<UserCallResponseDTO>(response.getResponse());
        auto functionCallResponse = std::get<FunctionCallResponseDTO>(userCallResponse.getResponse());
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
        FunctionCallArgumentDTO x((int64_t ) 1);
        FunctionCallArgumentDTO y((int64_t ) 1);
        FunctionCallArgumentDTO args[] = {x, y};

        FunctionCallRequestDTO functionCallRequest("NON_EXISTENT_FUNCTION", args, 2);
        UserCallRequestDTO ucReq(UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST, functionCallRequest);
        RequestDTO req(865, ucReq);
        MessageDTO reqMsg(CLIENT_AGENT_ID, CLIENT_AGENT_ID, req);

        // When
        m_clientSerializer->serializeToStream(reqMsg);

        MessageDTO responseMessage;
        m_clientDeserializer->deserializeFromStream(responseMessage);

        // Then
        auto response = std::get<ResponseDTO>(responseMessage.getMessage());
        auto userCallResponse = std::get<UserCallResponseDTO>(response.getResponse());
        auto functionCallResponse = std::get<FunctionCallResponseDTO>(userCallResponse.getResponse());
        GenericResponseStatusDTO status = functionCallResponse.getResponse().getStatus();
        std::string details = functionCallResponse.getResponse().getDetails();

        // Check response message
        ASSERT_EQ(response.getId(), 865);
        ASSERT_EQ(userCallResponse.getDestination(), UserCallTargetDTO::BUZZ);
        ASSERT_EQ(status, GenericResponseStatusDTO::Unknown);

        // Check that there were NO side effects.
        ASSERT_EQ(g_posX, 0);
        ASSERT_EQ(g_posY, 0);

        cleanUpAfterTest();
    }

    void testGetInstantaneousPayload() {
        // Given
        FunctionCallRequestDTO functionCallRequest("getInstantaneousPayload", nullptr, 0);
        UserCallRequestDTO ucReq(UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST, functionCallRequest);
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
        auto functionCallResponse = std::get<FunctionCallResponseDTO>(userCallResponse.getResponse());
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
        std::array<FunctionCallArgumentDTO, FUNCTION_ARGUMENT_COUNT> args = returnFunctionCallReq.getArguments();

        ASSERT_STREQ(returnFunctionName.c_str(), "getInstantaneousPayloadReturn");
        ASSERT_EQ(std::get<int64_t>(args[0].getArgument()), 1);

        cleanUpAfterTest();
    }

    void testGetDelayedPayload() {
        // Given
        FunctionCallRequestDTO functionCallRequest("getDelayedPayload", nullptr, 0);
        UserCallRequestDTO ucReq(UserCallTargetDTO::BUZZ, UserCallTargetDTO::HOST, functionCallRequest);
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
        auto functionCallResponse = std::get<FunctionCallResponseDTO>(userCallResponse.getResponse());
        GenericResponseStatusDTO status = functionCallResponse.getResponse().getStatus();
        std::string details = functionCallResponse.getResponse().getDetails();

        ASSERT_EQ(response.getId(), 865);
        ASSERT_EQ(userCallResponse.getDestination(), UserCallTargetDTO::BUZZ);
        ASSERT_EQ(status, GenericResponseStatusDTO::Ok);

        // Check return payload
        MessageDTO returnMessage;
        m_clientDeserializer->deserializeFromStream(returnMessage); // Should block until some data is available

        auto returnReq = std::get<RequestDTO>(returnMessage.getMessage());
        auto returnUcReq = std::get<UserCallRequestDTO>(returnReq.getRequest());
        auto returnFunctionCallReq = std::get<FunctionCallRequestDTO>(returnUcReq.getRequest());
        std::string returnFunctionName = returnFunctionCallReq.getFunctionName();
        std::array<FunctionCallArgumentDTO, FUNCTION_ARGUMENT_COUNT> args = returnFunctionCallReq.getArguments();

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
