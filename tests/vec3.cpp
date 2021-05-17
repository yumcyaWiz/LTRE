#include "LTRE/math/vec3.hpp"

#include "gtest/gtest.h"

using namespace LTRE;

TEST(Vec3Operator, BasicAssertions) {
  EXPECT_EQ(Vec3(0, 1, 2) + Vec3(3, 4, 5), Vec3(3, 5, 7));

  EXPECT_EQ(Vec3(0, 1, 2) - Vec3(3, 4, 5), Vec3(-3, -3, -3));

  EXPECT_EQ(Vec3(0, 1, 2) * 2, Vec3(0, 2, 4));
  EXPECT_EQ(Vec3(0, 1, 2) * 2, Vec3(0, 2, 4));

  EXPECT_EQ(Vec3(0, 2, 4) / 2, Vec3(0, 1, 2));
  EXPECT_EQ(4 / Vec3(1, 2, 4), Vec3(4, 2, 1));

  EXPECT_EQ(-Vec3(1, 2, 3), Vec3(-1, -2, -3));
}

TEST(Vec3Length, BasicAssertions) {
  EXPECT_EQ(length(Vec3(1, 2, 2)), 3);
  EXPECT_EQ(length2(Vec3(1, 2, 2)), 9);
}

TEST(Vec3Dot, BasicAssertions) {
  EXPECT_EQ(dot(Vec3(1, 2, 3), Vec3(4, 5, 6)), 32);
}

TEST(Vec3Cross, BasicAssertions) {
  EXPECT_EQ(cross(Vec3(1, 0, 0), Vec3(0, 1, 0)), Vec3(0, 0, 1));
}