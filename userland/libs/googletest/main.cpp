#include <utility>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::ElementsAre;
using testing::Pair;

TEST(MainTest, MyTest0) {
    EXPECT_EQ(1, 1);

    // Array comparison.
    // https://stackoverflow.com/questions/1460703/comparison-of-arrays-in-google-test
    ASSERT_THAT(
        (std::vector<int>{5, 10, 15}),
        ElementsAre(5, 10, 15)
    );
    ASSERT_THAT(
        (std::vector<std::pair<int, int>>{{1, -1}, {2, -2}}),
        ElementsAre(Pair(1, -1), Pair(2, -2))
    );
}

TEST(MainTest, MyTest1) {
    EXPECT_EQ(1, 1);
}
