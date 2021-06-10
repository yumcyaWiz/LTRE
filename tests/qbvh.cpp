#include "LTRE/intersector/qbvh.hpp"

#include <iostream>

#include "gtest/gtest.h"

using namespace LTRE;

TEST(SIMDIntersection, BasicAssertions) {
  const Ray ray(Vec3(0, 0, -10), Vec3(0, 0, 1));
  const Vec3 dirInv = 1.0f / ray.direction;
  int dirInvSign[3];
  for (int i = 0; i < 3; ++i) {
    dirInvSign[i] = dirInv[i] > 0 ? 0 : 1;
  }
  const AABB bounds1(Vec3(-1), Vec3(-0.5));
  const AABB bounds2(Vec3(-0.2), Vec3(0.2));
  const AABB bounds3(Vec3(0.3), Vec3(1));
  const AABB bounds4(Vec3(1), Vec3(2));

  const __m128 orig[3] = {_mm_set1_ps(ray.origin[0]),
                          _mm_set1_ps(ray.origin[1]),
                          _mm_set1_ps(ray.origin[2])};
  const __m128 _dirInv[3] = {_mm_set1_ps(dirInv[0]), _mm_set1_ps(dirInv[1]),
                             _mm_set1_ps(dirInv[2])};
  const __m128 raytmin = _mm_set_ps1(ray.tmin);
  const __m128 raytmax = _mm_set_ps1(ray.tmax);
  __m128 bounds[2][3];
  bounds[0][0] = _mm_set_ps(bounds1.bounds[0][0], bounds2.bounds[0][0],
                            bounds3.bounds[0][0], bounds4.bounds[0][0]);
  bounds[0][1] = _mm_set_ps(bounds1.bounds[0][1], bounds2.bounds[0][1],
                            bounds3.bounds[0][1], bounds4.bounds[0][1]);
  bounds[0][2] = _mm_set_ps(bounds1.bounds[0][2], bounds2.bounds[0][2],
                            bounds3.bounds[0][2], bounds4.bounds[0][2]);

  bounds[1][0] = _mm_set_ps(bounds1.bounds[1][0], bounds2.bounds[1][0],
                            bounds3.bounds[1][0], bounds4.bounds[1][0]);
  bounds[1][1] = _mm_set_ps(bounds1.bounds[1][1], bounds2.bounds[1][1],
                            bounds3.bounds[1][1], bounds4.bounds[1][1]);
  bounds[1][2] = _mm_set_ps(bounds1.bounds[1][2], bounds2.bounds[1][2],
                            bounds3.bounds[1][2], bounds4.bounds[1][2]);

  const int hitMask = QBVH<Primitive, BVHSplitStrategy::SAH>::intersectAABB(
      orig, _dirInv, dirInvSign, raytmin, raytmax, bounds);
  const int hitChild[4] = {hitMask & 0b1000, hitMask & 0b100, hitMask & 0b10,
                           hitMask & 0b1};

  EXPECT_EQ(hitChild[0], 0);
  EXPECT_GT(hitChild[1], 0);
  EXPECT_EQ(hitChild[2], 0);
  EXPECT_EQ(hitChild[3], 0);
}