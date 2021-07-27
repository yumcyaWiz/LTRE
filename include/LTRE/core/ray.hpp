#ifndef _LTRE_RAY_H
#define _LTRE_RAY_H

#include "LTRE/math/vec3.hpp"

namespace LTRE {

struct Ray {
  Vec3 origin;
  Vec3 direction;
  mutable float tmin;
  mutable float tmax;

  Ray();
  Ray(const Vec3& origin, const Vec3& direction);

  Vec3 operator()(float t) const;
};

}  // namespace LTRE

#endif