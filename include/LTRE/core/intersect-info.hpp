#ifndef _INTERSECT_INFO_H
#define _INTERSECT_INFO_H

#include "LTRE/core/vec3.hpp"

namespace LTRE {

struct IntersectInfo {
  float t;
  Vec3 hitPos;
  Vec3 hitNormal;
};

}  // namespace LTRE

#endif