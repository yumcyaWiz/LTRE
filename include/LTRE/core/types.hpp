#ifndef _INTERSECT_INFO_H
#define _INTERSECT_INFO_H
#include <iostream>

#include "LTRE/math/vec2.hpp"
#include "LTRE/math/vec3.hpp"

namespace LTRE {

class Primitive;

struct SurfaceInfo {
  Vec3 position;
  Vec3 normal;
  Vec2 uv;
};

struct IntersectInfo {
  float t;
  SurfaceInfo surfaceInfo;
  Vec2 barycentric;
  const Primitive* hitPrimitive;
};

}  // namespace LTRE

#endif