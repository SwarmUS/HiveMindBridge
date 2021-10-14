#include "hivemind-bridge/Callback.h"
#include "hivemind-bridge/UserCallbackMap.h"
#include <gmock/gmock.h>

class UserCallbackMapFixture : public testing::Test {
  protected:
    UserCallbackMap m_userCallbackMap;

    // Declare some test callbacks
    CallbackFunction m_testFunction =
        [&](const CallbackArgs& args) -> std::optional<CallbackReturn> { return {}; };

    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(UserCallbackMapFixture, registerNewCallback_Success) {
    ASSERT_FALSE(m_userCallbackMap.registerCallback("testFunction", m_testFunction));
}

TEST_F(UserCallbackMapFixture, registerOverwriteCallback_Success) {
    // Given
    m_userCallbackMap.registerCallback("testFunction", m_testFunction);

    // When
    // Then
    ASSERT_TRUE(m_userCallbackMap.registerCallback("testFunction", m_testFunction));
}

TEST_F(UserCallbackMapFixture, getCallback_Success) {
    // Given
    m_userCallbackMap.registerCallback("testFunction", m_testFunction);

    // When
    // Then
    ASSERT_TRUE(m_userCallbackMap.getCallback("testFunction"));
}

TEST_F(UserCallbackMapFixture, GetCallback_Fail) {
    ASSERT_FALSE(m_userCallbackMap.getCallback("Nonexisting"));
}

TEST_F(UserCallbackMapFixture, getLength_Success) {
    // Given
    m_userCallbackMap.registerCallback("testFunction1", m_testFunction);
    m_userCallbackMap.registerCallback("testFunction2", m_testFunction);
    m_userCallbackMap.registerCallback("testFunction3", m_testFunction);
    m_userCallbackMap.registerCallback("testFunction4", m_testFunction);

    // When
    // Then
    ASSERT_EQ(m_userCallbackMap.getLength(), 4);
}

TEST_F(UserCallbackMapFixture, getManifestAt_Success) {
    // Given
    CallbackArgsManifest expectedManifest;
    expectedManifest.push_back(
        UserCallbackArgumentDescription("x", FunctionDescriptionArgumentTypeDTO::Int));
    expectedManifest.push_back(
        UserCallbackArgumentDescription("y", FunctionDescriptionArgumentTypeDTO::Float));

    m_userCallbackMap.registerCallback("testFunction", m_testFunction, expectedManifest);

    // When
    auto actualManifest = m_userCallbackMap.getManifestAt(0).value();

    // Then
    ASSERT_EQ(actualManifest[0].getName(), "x");
    ASSERT_EQ(actualManifest[0].getType(), FunctionDescriptionArgumentTypeDTO::Int);

    ASSERT_EQ(actualManifest[1].getName(), "y");
    ASSERT_EQ(actualManifest[1].getType(), FunctionDescriptionArgumentTypeDTO::Float);
}

TEST_F(UserCallbackMapFixture, getManifestAt_OutOfBounds) {
    // Given
    CallbackArgsManifest expectedManifest;
    expectedManifest.push_back(
        UserCallbackArgumentDescription("x", FunctionDescriptionArgumentTypeDTO::Int));
    expectedManifest.push_back(
        UserCallbackArgumentDescription("y", FunctionDescriptionArgumentTypeDTO::Float));

    m_userCallbackMap.registerCallback("testFunction", m_testFunction, expectedManifest);

    // When
    auto actualManifest = m_userCallbackMap.getManifestAt(99);

    // Then
    ASSERT_FALSE(actualManifest);
}

TEST_F(UserCallbackMapFixture, getNameAt_Success) {
    // Given
    m_userCallbackMap.registerCallback("testFunction", m_testFunction);

    // When
    auto actualName = m_userCallbackMap.getNameAt(0).value();

    // Then
    ASSERT_STREQ(actualName.c_str(), "testFunction");
}

TEST_F(UserCallbackMapFixture, getNameAt_OutOfBounds) {
    // Given
    m_userCallbackMap.registerCallback("testFunction", m_testFunction);

    // When
    auto actualName = m_userCallbackMap.getNameAt(99);

    // Then
    ASSERT_FALSE(actualName);
}