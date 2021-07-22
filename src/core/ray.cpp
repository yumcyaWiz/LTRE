#include "LTRE/core/ray.hpp"

#include <limits>

namespace LTRE {

Ray::Ray() : tmin(RAY_EPS), tmax(std::numeric_limits<float>::max()) {}

Ray::Ray(const Vec3& origin, const Vec3& direction)
    : origin(origin),
      direction(direction),
      tmin(RAY_EPS),
      tmax(std::numeric_limits<float>::max()) {}

Vec3 Ray::operator()(float t) const { return origin + t * direction; }

}  // namespace LTRE