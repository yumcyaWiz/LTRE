#ifndef _LTRE_MICROFACET_H
#define _LTRE_MICROFACET_H
#include "LTRE/bsdf/bxdf/bxdf.hpp"
#include "LTRE/core/constant.hpp"
#include "LTRE/math/vec3.hpp"

namespace LTRE {

class MicrofacetDistribution {
 public:
  virtual float D(const Vec3& wh) const = 0;
  virtual float Lambda(const Vec3& w) const = 0;

  float G1(const Vec3& w) const { return 1.0f / (1.0f + Lambda(w)); }

  // height-correlated masking and shadowing
  float G(const Vec3& wo, const Vec3& wi) const {
    return 1.0f / (1.0f + Lambda(wo) + Lambda(wi));
  }
};

class Beckmann : public MicrofacetDistribution {
 private:
  float alphaX;
  float alphaY;

 public:
  Beckmann(float alphaX, float alphaY) : alphaX(alphaX), alphaY(alphaY) {}

  float D(const Vec3& wh) const override {
    const float tan2Theta = BxDF::tan2Theta(wh);
    if (std::isinf(tan2Theta)) return 0;
    const float cos4Theta = BxDF::cos2Theta(wh) * BxDF::cos2Theta(wh);
    return std::exp(-tan2Theta * (BxDF::cos2Phi(wh) / (alphaX * alphaX) +
                                  BxDF::sin2Phi(wh) / (alphaY * alphaY))) /
           (PI * alphaX * alphaY * cos4Theta);
  }

  float Lambda(const Vec3& w) const override {
    const float absTanTheta = BxDF::absTanTheta(w);
    if (std::isinf(absTanTheta)) return 0;

    const float alpha = std::sqrt(BxDF::cos2Phi(w) * alphaX * alphaX +
                                  BxDF::sin2Phi(w) * alphaY * alphaY);
    const float a = 1.0f / (alpha * absTanTheta);
    if (a >= 1.6f) {
      return 0;
    }
    return (1.0f - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
  }
};

}  // namespace LTRE

#endif