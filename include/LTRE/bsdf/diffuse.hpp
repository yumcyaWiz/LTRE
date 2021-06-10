#ifndef _LTRE_LAMBERT_H
#define _LTRE_LAMBERT_H
#include "LTRE/bsdf/bsdf.hpp"
#include "LTRE/core/texture.hpp"
#include "LTRE/sampling/sampling.hpp"

namespace LTRE {

class Lambert : public BSDF {
 private:
  const std::shared_ptr<Texture<Vec3>> rho;

 public:
  Lambert(const std::shared_ptr<Texture<Vec3>>& rho) : rho(rho) {}

  Vec3 baseColor(const IntersectInfo& info) const override {
    return rho->sample(info);
  }

  Vec3 bsdf([[maybe_unused]] const Vec3& wo, const IntersectInfo& info,
            [[maybe_unused]] const Vec3& wi) const override {
    return rho->sample(info) / PI;
  }
  Vec3 sample([[maybe_unused]] const Vec3& wo, const IntersectInfo& info,
              Sampler& sampler, Vec3& wi, float& pdf) const {
    wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return bsdf(wo, info, wi);
  }
};

class OrenNayer : public BSDF {
 private:
  const std::shared_ptr<Texture<Vec3>> rho;
  float A;
  float B;

 public:
  OrenNayer(const std::shared_ptr<Texture<Vec3>>& rho, float sigma) : rho(rho) {
    const float sigma2 = sigma * sigma;
    A = 1.0f - sigma2 / (2.0f * (sigma2 + 0.33f));
    B = 0.45 * sigma2 / (sigma2 + 0.99f);
  }

  Vec3 baseColor(const IntersectInfo& info) const override {
    return rho->sample(info);
  }

  Vec3 bsdf(const Vec3& wo, const IntersectInfo& info,
            const Vec3& wi) const override {
    // compute max(0, cos(phi_i - phi_o))
    const float sinPhiI = BSDF::sinPhi(wi);
    const float cosPhiI = BSDF::cosPhi(wi);
    const float sinPhiO = BSDF::sinPhi(wo);
    const float cosPhiO = BSDF::cosPhi(wo);
    const float maxCos = std::max(0.0f, cosPhiI * cosPhiO + sinPhiI * sinPhiO);

    // compute sin(alpha), tan(beta)
    const float sinThetaI = BSDF::sinTheta(wi);
    const float sinThetaO = BSDF::sinTheta(wo);
    float sinAlpha, tanBeta;
    if (absCosTheta(wi) > absCosTheta(wo)) {
      sinAlpha = sinThetaO;
      tanBeta = sinThetaI / absCosTheta(wi);
    } else {
      sinAlpha = sinThetaI;
      tanBeta = sinThetaO / absCosTheta(wo);
    }

    return rho->sample(info) * PI_INV * (A + B * maxCos * sinAlpha * tanBeta);
  }

  Vec3 sample(const Vec3& wo, const IntersectInfo& info, Sampler& sampler,
              Vec3& wi, float& pdf) const {
    wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return bsdf(wo, info, wi);
  }
};

class DisneyDiffuse : public BSDF {
 private:
  Lambert lambert;
  float roughness;

 public:
  DisneyDiffuse(const std::shared_ptr<Texture<Vec3>>& rho, float roughness)
      : lambert{rho}, roughness{roughness} {}

  Vec3 baseColor(const IntersectInfo& info) const override {
    return lambert.baseColor(info);
  }

  Vec3 bsdf(const Vec3& wo, const IntersectInfo& info,
            const Vec3& wi) const override {
    const Vec3 h = normalize(wo + wi);
    const float cosD = dot(h, wi);
    const float f90 = 0.5f + 2.0f * roughness * cosD * cosD;
    return lambert.bsdf(wo, info, wi) * schlickFresnelT(wi, f90) *
           schlickFresnelT(wo, f90);
  }

  Vec3 sample(const Vec3& wo, const IntersectInfo& info, Sampler& sampler,
              Vec3& wi, float& pdf) const {
    wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return bsdf(wo, info, wi);
  }
};

}  // namespace LTRE

#endif