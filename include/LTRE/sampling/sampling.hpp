#ifndef _LTRE_SAMPLING_H
#define _LTRE_SAMPLING_H
#include <algorithm>

#include "LTRE/core/constant.hpp"
#include "LTRE/math/vec2.hpp"

namespace LTRE {

inline Vec3 sampleHemisphere(const Vec2& uv, float& pdf) {
  const float theta = std::acos(std::max(1.0f - uv[0], 0.0f));
  const float phi = PI_MUL_2 * uv[1];
  pdf = PI_MUL_2_INV;
  return sphericalToCartesian(theta, phi);
}

inline Vec3 sampleCosineHemisphere(const Vec2& uv, float& pdf) {
  const float theta =
      0.5f * std::acos(std::clamp(1.0f - 2.0f * uv[0], -1.0f, 1.0f));
  const float phi = PI_MUL_2 * uv[1];
  const float cosTheta = std::cos(theta);
  pdf = PI_INV * cosTheta;
  return sphericalToCartesian(theta, phi);
}

}  // namespace LTRE

#endif