#ifndef _LTRE_LAMBERT_H
#define _LTRE_LAMBERT_H
#include "LTRE/bsdf/bxdf/bxdf.hpp"
#include "LTRE/core/texture.hpp"
#include "LTRE/sampling/sampling.hpp"

namespace LTRE {

class Lambert : public BxDF {
 private:
  Vec3 rho;

 public:
  Lambert() {}

  Vec3 f([[maybe_unused]] const Vec3& wo,
         [[maybe_unused]] const Vec3& wi) const override {
    return rho / PI;
  }

  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
              float& pdf) const override {
    wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return f(wo, wi);
  }
};

class OrenNayer : public BxDF {
 private:
  Vec3 rho;
  float sigma;
  float A;
  float B;

 public:
  OrenNayer(const Vec3& rho, float sigma) : rho(rho), sigma(sigma) {
    // compute params
    const float sigma2 = sigma * sigma;
    A = 1.0f - sigma2 / (2.0f * (sigma2 + 0.33f));
    B = 0.45 * sigma2 / (sigma2 + 0.99f);
  }

  Vec3 f(const Vec3& wo, const Vec3& wi) const override {
    // compute max(0, cos(phi_i - phi_o))
    const float sinPhiI = sinPhi(wi);
    const float cosPhiI = cosPhi(wi);
    const float sinPhiO = sinPhi(wo);
    const float cosPhiO = cosPhi(wo);
    const float maxCos = std::max(0.0f, cosPhiI * cosPhiO + sinPhiI * sinPhiO);

    // compute sin(alpha), tan(beta)
    const float sinThetaI = sinTheta(wi);
    const float sinThetaO = sinTheta(wo);
    float sinAlpha, tanBeta;
    if (absCosTheta(wi) > absCosTheta(wo)) {
      sinAlpha = sinThetaO;
      tanBeta = sinThetaI / absCosTheta(wi);
    } else {
      sinAlpha = sinThetaI;
      tanBeta = sinThetaO / absCosTheta(wo);
    }

    return rho * PI_INV * (A + B * maxCos * sinAlpha * tanBeta);
  }

  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
              float& pdf) const override {
    wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return f(wo, wi);
  }
};

}  // namespace LTRE

#endif