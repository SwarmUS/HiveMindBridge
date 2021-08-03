#include "../utils/HiveMindBridgeFixture.h"
#include "../utils/Logger.h"
#include "../utils/TCPClient.h"
#include "hivemind-bridge/HiveMindBridge.h"
#include <gmock/gmock.h>
#include <thread>

class HiveMindHostApiResponseIntegrationTestFixture : public testing::Test, public HiveMindBridgeFixture {
    void SetUp() override {
        std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_DELAY_MS));
    }

    void TearDown() override { cleanUpAfterTest(); };

  public:
    HiveMindHostApiResponseIntegrationTestFixture() = default;

    int m_functionCalledCount = 0;

    // Teardown method that needs to be run manually since we run everything inside a single test
    // case.
    void cleanUpAfterTest() {
        m_functionCalledCount = 0;
    }

    void testHandleNeighborListUpdate() {
        // Given
        uint16_t neighbors[] = {1, 2, 3, 4};
        GetNeighborsListResponseDTO list(neighbors, 4);
        HiveMindHostApiResponseDTO hmResp(list);
        ResponseDTO response(42, hmResp);
        MessageDTO incomingMessage(0, 0, response);

        m_bridge->onNeighborListUpdated([&](std::array<uint16_t, NEIGHBORS_LIST_SIZE> list, uint16_t neighborsLength) {
            ASSERT_EQ(neighborsLength, 4);
            ASSERT_THAT(list, testing::ElementsAre(1,2,3,4));
            m_functionCalledCount++;
        });

        // When
        m_clientSerializer->serializeToStream(incomingMessage);

        // Then

        ASSERT_EQ(m_functionCalledCount, 1);

        cleanUpAfterTest();
    }

    void testHandleNeighborUpdate() {
//        ASSERT_TRUE(false);

        cleanUpAfterTest();
    }
};

TEST_F(HiveMindHostApiResponseIntegrationTestFixture, hiveMindHostApiResponseIntegrationTest) {
    testHandleNeighborListUpdate();
    testHandleNeighborUpdate();
}