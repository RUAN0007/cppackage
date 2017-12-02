#include "gtest.h"

TEST(GTest, Normal) {
  EXPECT_EQ(1, 1);
  ASSERT_EQ(1, 1);
}
