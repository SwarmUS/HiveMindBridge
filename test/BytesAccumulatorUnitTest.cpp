#include "hivemind-bridge/BytesAccumulator.h"
#include <gmock/gmock.h>

class BytesAccumulatorFixture : public testing::Test {
  protected:
    BytesAccumulator m_bytesAccumulator;
};

TEST_F(BytesAccumulatorFixture, appendBytes_success) {
    // Given
    uint8_t initialBytes[4] = {0, 1, 2, 3};
    uint8_t appendedBytes[12] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    uint8_t expectedBytes[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    // When
    m_bytesAccumulator.appendBytes(initialBytes, 4, 0);
    m_bytesAccumulator.appendBytes(appendedBytes, 12, 1);

    // Then
    std::vector<uint8_t>* accumulatedBytes = m_bytesAccumulator.getBytes();
    ASSERT_EQ(memcmp(expectedBytes, accumulatedBytes->data(), 16), 0);
}