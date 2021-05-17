#include "LTRE/math/vec2.hpp"

#include <cmath>

#include "gtest/gtest.h"

using namespace LTRE;

TEST(Vec2Operator, BasicAssertions) {
  EXPECT_EQ(Vec2(1, 2) + Vec2(4, 5), Vec2(5, 7));

  EXPECT_EQ(Vec2(1, 2) - Vec2(4, 5), Vec2(-3, -3));

  EXPECT_EQ(Vec2(1, 2) * 2, Vec2(2, 4));
  EXPECT_EQ(Vec2(1, 2) * 2, Vec2(2, 4));

  EXPECT_EQ(Vec2(2, 4) / 2, Vec2(1, 2));
  EXPECT_EQ(4 / Vec2(2, 4), Vec2(2, 1));

  EXPECT_EQ(-Vec2(2, 3), Vec2(-2, -3));
}

TEST(Vec2Length, BasicAssertions) {
  EXPECT_EQ(length(Vec2(3, 4)), 5);
  EXPECT_EQ(length2(Vec2(2, 3)), 13);
}

TEST(Vec2Dot, BasicAssertions) { EXPECT_EQ(dot(Vec2(2, 3), Vec2(5, 6)), 28); }