#include "hivemind-bridge/HiveMindHostApiResponseHandler.h"
#include "pheromones/MessageDTO.h"
#include "pheromones/PheromonesSettings.h"
#include "utils/Logger.h"
#include <gmock/gmock.h>

class HiveMindHostApiResponseHandlerFixture : public testing::Test {
  protected:
    Logger m_logger;
    std::unique_ptr<HiveMindHostApiResponseHandler> m_hmResponseHandler;

    // Value to test side effects
    int m_functionCalledCount = 0;

    void SetUp() override {
        m_hmResponseHandler = std::make_unique<HiveMindHostApiResponseHandler>(m_logger);
        m_functionCalledCount = 0;
    }
};

TEST_F(HiveMindHostApiResponseHandlerFixture, test_onNeighborListUpdated_noOverwrite) {
    // Given

    // When
    bool wasOverwritten = m_hmResponseHandler->onNeighborListUpdated(
        [](std::array<uint16_t, NEIGHBORS_LIST_SIZE> neighborList, uint16_t neighborsLength) {});

    // Then
    ASSERT_FALSE(wasOverwritten);
}

TEST_F(HiveMindHostApiResponseHandlerFixture, test_onNeighborListUpdated_overwrite) {
    // Given

    // When
    m_hmResponseHandler->onNeighborListUpdated(
        [](std::array<uint16_t, NEIGHBORS_LIST_SIZE> neighborList, uint16_t neighborsLength) {});
    bool wasOverwritten = m_hmResponseHandler->onNeighborListUpdated(
        [](std::array<uint16_t, NEIGHBORS_LIST_SIZE> neighborList, uint16_t neighborsLength) {});

    // Then
    ASSERT_TRUE(wasOverwritten);
}

TEST_F(HiveMindHostApiResponseHandlerFixture, test_onNeighborUpdated_noOverwrite) {
    // Given

    // When
    bool wasOverwritten = m_hmResponseHandler->onNeighborUpdated(
        [](uint16_t neighborId, std::optional<Position> position) {});

    // Then
    ASSERT_FALSE(wasOverwritten);
}

TEST_F(HiveMindHostApiResponseHandlerFixture, test_onNeighborUpdated_overwrite) {
    // Given

    // When
    m_hmResponseHandler->onNeighborUpdated(
        [](uint16_t neighborId, std::optional<Position> position) {});
    bool wasOverwritten = m_hmResponseHandler->onNeighborUpdated(
        [](uint16_t neighborId, std::optional<Position> position) {});

    // Then
    ASSERT_TRUE(wasOverwritten);
}

TEST_F(HiveMindHostApiResponseHandlerFixture, test_handleNeighborList_failNoCallback) {
    // Given
    uint16_t neighbors[] = {1, 2, 3, 4};
    GetNeighborsListResponseDTO list(neighbors, 4);
    HiveMindHostApiResponseDTO hmResp(list);
    ResponseDTO response(42, hmResp);
    MessageDTO incomingMessage(0, 0, response);

    // When
    m_hmResponseHandler->handleMessage(incomingMessage, hmResp);

    // Then
    ASSERT_EQ(m_functionCalledCount, 0);
}

TEST_F(HiveMindHostApiResponseHandlerFixture, test_handleNeighborList_success) {
    // Given
    uint16_t neighbors[] = {1, 2, 3, 4};
    GetNeighborsListResponseDTO list(neighbors, 4);
    HiveMindHostApiResponseDTO hmResp(list);
    ResponseDTO response(42, hmResp);
    MessageDTO incomingMessage(0, 0, response);

    m_hmResponseHandler->onNeighborListUpdated(
        [&](std::array<uint16_t, NEIGHBORS_LIST_SIZE> list, uint16_t neighborsLength) {
            ASSERT_EQ(neighborsLength, 4);
            ASSERT_EQ(0, memcmp(list.data(), neighbors, 4));
            m_functionCalledCount++;
        });

    // When
    m_hmResponseHandler->handleMessage(incomingMessage, hmResp);

    // Then
    ASSERT_EQ(m_functionCalledCount, 1);
}

TEST_F(HiveMindHostApiResponseHandlerFixture, test_handleNeighbor_failNoCallback) {
    // Given
    GetNeighborResponseDTO neighbor(99, {});
    HiveMindHostApiResponseDTO hmResp(neighbor);
    ResponseDTO response(42, hmResp);
    MessageDTO incomingMessage(0, 0, response);

    // When
    m_hmResponseHandler->handleMessage(incomingMessage, hmResp);

    // Then
    ASSERT_EQ(m_functionCalledCount, 0);
}

TEST_F(HiveMindHostApiResponseHandlerFixture, test_handleNeighborWithPosition_success) {
    // Given
    NeighborPositionDTO pos(12, 37, true);
    GetNeighborResponseDTO neighbor(99, pos);
    HiveMindHostApiResponseDTO hmResp(neighbor);
    ResponseDTO response(42, hmResp);
    MessageDTO incomingMessage(0, 0, response);

    m_hmResponseHandler->onNeighborUpdated(
        [&](uint16_t neighborId, std::optional<Position> position) {
            ASSERT_TRUE(position);

            ASSERT_EQ(position->distance, 12);
            ASSERT_EQ(position->relativeOrientation, 37);
            ASSERT_TRUE(position->inLOS);
            m_functionCalledCount++;
        });

    // When
    m_hmResponseHandler->handleMessage(incomingMessage, hmResp);

    // Then
    ASSERT_EQ(m_functionCalledCount, 1);
}

TEST_F(HiveMindHostApiResponseHandlerFixture, test_handleNeighborNoPosition_success) {
    // Given
    GetNeighborResponseDTO neighbor(99, {});
    HiveMindHostApiResponseDTO hmResp(neighbor);
    ResponseDTO response(42, hmResp);
    MessageDTO incomingMessage(0, 0, response);

    m_hmResponseHandler->onNeighborUpdated(
        [&](uint16_t neighborId, std::optional<Position> position) {
            m_functionCalledCount++;

            ASSERT_FALSE(position);
        });

    // When
    m_hmResponseHandler->handleMessage(incomingMessage, hmResp);

    // Then
    ASSERT_EQ(m_functionCalledCount, 1);
}