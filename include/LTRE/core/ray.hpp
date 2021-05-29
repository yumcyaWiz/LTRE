#ifndef _LTRE_RAY_H
#define _LTRE_RAY_H
#include <limits>

#include "LTRE/math/vec3.hpp"

namespace LTRE {

struct Ray {
  Vec3 origin;
  Vec3 direction;
  mutable float tmin;
  mutable float tmax;
  static constexpr float RAY_EPS = 1e-3f;

  Ray() : tmin(RAY_EPS), tmax(std::numeric_limits<float>::max()) {}

  Ray(const Vec3& origin, const Vec3& direction)
      : origin(origin),
        direction(direction),
        tmin(RAY_EPS),
        tmax(std::numeric_limits<float>::max()) {}

  Vec3 operator()(float t) const { return origin + t * direction; }
};

}  // namespace LTRE

#endif