#include "hivemind-bridge/Callback.h"
#include "hivemind-bridge/HiveMindBridgeImpl.h"
#include "hivemind-bridge/UserCallRequestHandler.h"
#include "hivemind-bridge/UserCallbackMap.h"
#include "mocks/HiveMindHostApiRequestHandlerInterfaceMock.h"
#include "mocks/HiveMindHostDeserializerInterfaceMock.h"
#include "mocks/HiveMindHostSerializerInterfaceMock.h"
#include "mocks/MessageHandlerInterfaceMock.h"
#include "mocks/TCPServerInterfaceMock.h"
#include "mocks/ThreadSafeQueueInterfaceMock.h"
#include "mocks/UserCallRequestManagerInterfaceMock.h"
#include "utils/BytesTestData.h"
#include "utils/Logger.h"
#include <cmath>
#include <gmock/gmock.h>

std::function<std::optional<CallbackReturn>()> g_validCallbackWithInstantReturn =
    []() -> std::optional<CallbackReturn> {
    CallbackArgs returnArgs;
    returnArgs[0] = FunctionCallArgumentDTO(1.0f);

    CallbackReturn cbReturn("instantReturn", returnArgs);
    return cbReturn;
};

std::function<std::optional<CallbackReturn>()> g_validCallbackWithoutInstantReturn =
    []() -> std::optional<CallbackReturn> { return {}; };

class HiveMindBridgeImplUnitFixture : public testing::Test {
  protected:
    Logger m_logger;
    TCPServerInterfaceMock m_tcpServer;
    HiveMindHostDeserializerInterfaceMock m_deserializer;
    HiveMindHostSerializerInterfaceMock m_serializer;
    HiveMindBridgeImpl* m_hivemindBridge;
    ThreadSafeQueueInterfaceMock<MessageDTO> m_inboundQueue;
    ThreadSafeQueueInterfaceMock<OutboundRequestHandle> m_outboundQueue;
    MessageHandlerInterfaceMock m_messageHandler;
    UserCallRequestManagerInterfaceMock m_userCallRequestHandler;
    HiveMindHostApiRequestHandlerInterfaceMock m_hmHostApiRequestHandler;
    UserCallbackMap m_userCallbackMap;

    InboundRequestHandle m_validResultWithReturn;
    MessageDTO m_dummyResponseMessage = MessageUtils::createResponseMessage(
        1, 1, 1, UserCallTargetDTO::UNKNOWN, GenericResponseStatusDTO::Ok, "");

    void SetUp() {
        m_hivemindBridge = new HiveMindBridgeImpl(
            m_tcpServer, m_serializer, m_deserializer, m_userCallRequestHandler,
            m_hmHostApiRequestHandler, m_userCallbackMap, m_messageHandler, m_inboundQueue,
            m_outboundQueue, m_logger);
    }

    void TearDown() { delete m_hivemindBridge; }
};

TEST_F(HiveMindBridgeImplUnitFixture, spinInstantaneousCallback_WithReturn) {
    // Given
    m_validResultWithReturn.setCallbackReturnContext(
        std::async(std::launch::async, g_validCallbackWithInstantReturn).share());
    m_validResultWithReturn.setResponse(m_dummyResponseMessage);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(250)); // Just to make sure the callback ends before test

    // When
    EXPECT_CALL(m_tcpServer, isClientConnected()).WillOnce(testing::Return(true));
    EXPECT_CALL(m_inboundQueue, empty()).WillOnce(testing::Return(false));
    EXPECT_CALL(m_inboundQueue, front());
    EXPECT_CALL(m_messageHandler, handleMessage(testing::_))
        .WillOnce(testing::Return(m_validResultWithReturn));
    EXPECT_CALL(m_inboundQueue, pop());
    EXPECT_CALL(m_serializer, serializeToStream(testing::_)).Times(2); // ack + return
    EXPECT_CALL(m_tcpServer, close()).Times(1);

    m_hivemindBridge->spin();

    // Then
}

TEST_F(HiveMindBridgeImplUnitFixture, spinInstantaneousCallback_WithoutReturn) {
    // Given
    m_validResultWithReturn.setCallbackReturnContext(
        std::async(std::launch::async, g_validCallbackWithoutInstantReturn).share());
    m_validResultWithReturn.setResponse(m_dummyResponseMessage);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(250)); // Just to make sure the callback ends before test

    // When
    EXPECT_CALL(m_tcpServer, isClientConnected()).WillOnce(testing::Return(true));
    EXPECT_CALL(m_inboundQueue, empty()).WillOnce(testing::Return(false));
    EXPECT_CALL(m_inboundQueue, front());
    EXPECT_CALL(m_messageHandler, handleMessage(testing::_))
        .WillOnce(testing::Return(m_validResultWithReturn));
    EXPECT_CALL(m_inboundQueue, pop());
    EXPECT_CALL(m_serializer, serializeToStream(testing::_)).Times(1); // ack only
    EXPECT_CALL(m_tcpServer, close()).Times(1);

    m_hivemindBridge->spin();

    // Then
}

TEST_F(HiveMindBridgeImplUnitFixture, spinGreetSuccess) {
    // Given

    // When
    testing::Sequence seq;
    EXPECT_CALL(m_tcpServer, isClientConnected()).InSequence(seq).WillOnce(testing::Return(false));
    EXPECT_CALL(m_tcpServer, listen()).Times(1);
    EXPECT_CALL(m_tcpServer, isClientConnected()).InSequence(seq).WillOnce(testing::Return(true));
    EXPECT_CALL(m_serializer, serializeToStream(testing::_)).Times(1);
    EXPECT_CALL(m_deserializer, deserializeFromStream(testing::_)).Times(1);
    EXPECT_CALL(m_messageHandler, handleGreet(testing::_))
        .WillOnce(testing::Return(std::optional<uint32_t>(42)));
    EXPECT_CALL(m_tcpServer, isClientConnected())
        .InSequence(seq)
        .WillRepeatedly(testing::Return(false)); // to make sure the thread ends
    EXPECT_CALL(m_tcpServer, close()).Times(1);

    m_hivemindBridge->spin();

    // Then
    ASSERT_EQ(m_hivemindBridge->getSwarmAgentId(), 42);
}

TEST_F(HiveMindBridgeImplUnitFixture, spinGreetFail) {
    // Given

    // When
    testing::Sequence seq;
    EXPECT_CALL(m_tcpServer, isClientConnected()).InSequence(seq).WillOnce(testing::Return(false));
    EXPECT_CALL(m_tcpServer, listen()).Times(1);
    EXPECT_CALL(m_tcpServer, isClientConnected()).InSequence(seq).WillOnce(testing::Return(true));
    EXPECT_CALL(m_serializer, serializeToStream(testing::_)).Times(1);
    EXPECT_CALL(m_deserializer, deserializeFromStream(testing::_)).Times(1);
    EXPECT_CALL(m_messageHandler, handleGreet(testing::_))
        .WillOnce(testing::Return(std::optional<uint32_t>()));
    EXPECT_CALL(m_tcpServer, close()).Times(2);

    m_hivemindBridge->spin();

    // Then
    ASSERT_EQ(m_hivemindBridge->getSwarmAgentId(), 0); // Default value
}

TEST_F(HiveMindBridgeImplUnitFixture, queueAndSendSuccess) {
    // Given
    MessageDTO msg = MessageUtils::createFunctionCallRequest(1, 1, 1, UserCallTargetDTO::BUZZ,
                                                             "someRemoteCallback");

    // When
    EXPECT_CALL(m_tcpServer, isClientConnected()).WillOnce(testing::Return(true));
    EXPECT_CALL(m_outboundQueue, push(testing::_)).Times(1);
    EXPECT_CALL(m_tcpServer, close()).Times(1);
    bool actual = m_hivemindBridge->queueAndSend(msg);

    // Then
    ASSERT_TRUE(actual);
}

TEST_F(HiveMindBridgeImplUnitFixture, queueAndSendFail) {
    // Given
    MessageDTO msg = MessageUtils::createFunctionCallRequest(1, 1, 1, UserCallTargetDTO::BUZZ,
                                                             "someRemoteCallback");

    // When
    EXPECT_CALL(m_tcpServer, isClientConnected()).WillOnce(testing::Return(false));
    EXPECT_CALL(m_tcpServer, close()).Times(1);
    bool actual = m_hivemindBridge->queueAndSend(msg);

    // Then
    ASSERT_FALSE(actual);
}

TEST_F(HiveMindBridgeImplUnitFixture, sendBytesTrivial) {
    // Given

    // When
    EXPECT_CALL(m_tcpServer, isClientConnected()).WillOnce(testing::Return(true));
    EXPECT_CALL(m_outboundQueue, push(testing::_)).Times(1);
    EXPECT_CALL(m_tcpServer, close()).Times(1);
    bool actual = m_hivemindBridge->sendBytes(42, SMALL_BYTE_ARRAY.arr, SMALL_BYTE_ARRAY_SIZE);

    // Then
    ASSERT_TRUE(actual);
}

TEST_F(HiveMindBridgeImplUnitFixture, sendBytesLongPayload) {
    // Given
    int expectedNumberOfPackets = std::ceil((float)LONG_BYTE_ARRAY_SIZE / 200);

    // When
    EXPECT_CALL(m_tcpServer, isClientConnected()).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(m_outboundQueue, push(testing::_)).Times(expectedNumberOfPackets);
    EXPECT_CALL(m_tcpServer, close()).Times(1);
    bool actual = m_hivemindBridge->sendBytes(42, LONG_BYTE_ARRAY.arr, LONG_BYTE_ARRAY_SIZE);

    // Then
    ASSERT_TRUE(actual);
}
