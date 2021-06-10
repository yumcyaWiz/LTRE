#ifndef _LTRE_BXDF_H
#define _LTRE_BXDF_H
#include <algorithm>

#include "LTRE/math/vec2.hpp"
#include "LTRE/math/vec3.hpp"
#include "LTRE/sampling/sampler.hpp"

namespace LTRE {

class Fresnel {
 private:
  const float ior1;
  const float ior2;

 public:
  Fresnel(float ior1, float ior2) : ior1(ior1), ior2(ior2) {}

  float evaluate(float cos) const {
    const auto pow5 = [](float x) { return x * x * x * x * x; };

    bool isEntering = cos < 0.0f;
    float n1 = ior1;
    float n2 = ior2;
    if (!isEntering) {
      std::swap(n1, n2);
    }
    cos = std::abs(cos);

    const float f0 = (n1 - n2) / (n1 + n2) * (n1 - n2) / (n1 + n2);
    return f0 + (1.0f - f0) * pow5(1.0f - cos);
  }
};

class BxDF {
 public:
  static float cosTheta(const Vec3& w) { return w[1]; }
  static float cos2Theta(const Vec3& w) { return w[1] * w[1]; }
  static float absCosTheta(const Vec3& w) { return std::abs(w[1]); }

  static float sin2Theta(const Vec3& w) {
    return std::max(0.0f, 1.0f - cos2Theta(w));
  }
  static float sinTheta(const Vec3& w) { return std::sqrt(sin2Theta(w)); }
  static float absSinTheta(const Vec3& w) { return std::abs(sinTheta(w)); }

  static float tanTheta(const Vec3& w) { return sinTheta(w) / cosTheta(w); }
  static float tan2Theta(const Vec3& w) { return sin2Theta(w) / cos2Theta(w); }
  static float absTanTheta(const Vec3& w) { return std::abs(tanTheta(w)); }

  static float cosPhi(const Vec3& w) {
    float sin = sinTheta(w);
    return (sin == 0) ? 1 : std::clamp(w[0] / sin, -1.0f, 1.0f);
  }
  static float sinPhi(const Vec3& w) {
    float sin = sinTheta(w);
    return (sin == 0) ? 0 : std::clamp(w[2] / sin, -1.0f, 1.0f);
  }
  static float cos2Phi(const Vec3& w) { return cosPhi(w) * cosPhi(w); }
  static float sin2Phi(const Vec3& w) { return sinPhi(w) * sinPhi(w); }

  virtual Vec3 f(const Vec3& wo, const Vec3& wi) const = 0;
  virtual Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
                      float& pdf) const = 0;
};

}  // namespace LTRE

#endif