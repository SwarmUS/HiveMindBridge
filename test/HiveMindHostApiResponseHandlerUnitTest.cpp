#include "utils/Logger.h"
#include <gmock/gmock.h>

class HiveMindHostApiResponseHandlerFixture : public testing::Test {
  protected:
    Logger m_logger;
};

TEST_F(HiveMindHostApiResponseHandlerFixture, test_) {
    ASSERT_TRUE(true);
}