#include <gtest/gtest.h>

TEST(FailTest, ThisTestPasses) {
  EXPECT_EQ(1, 1);
}

TEST(FailTest, ThisTestFails) {
  EXPECT_EQ(1, 2);
}
