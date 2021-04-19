#include <gmock/gmock.h>
#include "hivemind-bridge/Callback.h"
#include "hivemind-bridge/user-call/UserCallbackMap.h"

class UserCallbackMapFixture : public testing::Test {
protected:
    UserCallbackMap m_userCallbackMap;

    // Declare some test callbacks
    CallbackFunction m_testFunction = [&](CallbackArgs args,
                                          int argsLength) -> std::optional<CallbackReturn> {
        return {};
    };

    void SetUp() override {

    }

    void TearDown() override {

    }
};

TEST_F(UserCallbackMapFixture, registerNewCallback_Success) {
    ASSERT_FALSE(m_userCallbackMap.registerCallback("TestFunctionCallRequestDTO", m_testFunction));
}

TEST_F(UserCallbackMapFixture, registerOverwriteCallback_Success) {
    m_userCallbackMap.registerCallback("TestFunctionCallRequestDTO", m_testFunction);
    ASSERT_TRUE(m_userCallbackMap.registerCallback("TestFunctionCallRequestDTO", m_testFunction));
}

TEST_F(UserCallbackMapFixture, testGetCallbackSuccess) {
    m_userCallbackMap.registerCallback("TestFunctionCallRequestDTO", m_testFunction);
    ASSERT_TRUE(m_userCallbackMap.getCallback("TestFunctionCallRequestDTO"));
}

TEST_F(UserCallbackMapFixture, testGetCallbackFail) {
    ASSERT_FALSE(m_userCallbackMap.getCallback("Nonexisting"));
}
