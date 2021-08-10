#ifndef _LTRE_BXDF_H
#define _LTRE_BXDF_H
#include <algorithm>

#include "LTRE/math/vec2.hpp"
#include "LTRE/math/vec3.hpp"
#include "LTRE/sampling/sampler.hpp"

namespace LTRE {

class BxDF {
 public:
  static float cosTheta(const Vec3& w);
  static float cos2Theta(const Vec3& w);
  static float absCosTheta(const Vec3& w);

  static float sin2Theta(const Vec3& w);
  static float sinTheta(const Vec3& w);
  static float absSinTheta(const Vec3& w);

  static float tanTheta(const Vec3& w);
  static float tan2Theta(const Vec3& w);
  static float absTanTheta(const Vec3& w);

  static float cosPhi(const Vec3& w);
  static float sinPhi(const Vec3& w);
  static float cos2Phi(const Vec3& w);
  static float sin2Phi(const Vec3& w);

  static Vec3 reflect(const Vec3& v, const Vec3& n);
  static bool refract(const Vec3& v, const Vec3& n, float iorI, float iorT,
                      Vec3& t);

  virtual Vec3 f(const Vec3& wo, const Vec3& wi) const = 0;
  virtual Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
                      float& pdf) const = 0;
  virtual float pdf(const Vec3& wo, const Vec3& wi) const = 0;
};

}  // namespace LTRE

#endif