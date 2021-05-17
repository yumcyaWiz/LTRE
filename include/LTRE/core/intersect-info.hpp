#ifndef _INTERSECT_INFO_H
#define _INTERSECT_INFO_H

#include "LTRE/math/vec2.hpp"
#include "LTRE/math/vec3.hpp"

namespace LTRE {

struct IntersectInfo {
  float t;
  Vec3 hitPos;
  Vec3 hitNormal;
  Vec2 uv;
  unsigned int primID;
  unsigned int geomID;
};

}  // namespace LTRE

#endif