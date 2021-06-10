#ifndef _LTRE_DISNEY_H
#define _LTRE_DISNEY_H
#include "LTRE/bsdf/bsdf.hpp"
#include "LTRE/bsdf/diffuse.hpp"
#include "LTRE/core/texture.hpp"
#include "LTRE/sampling/sampling.hpp"

namespace LTRE {

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

class DisneySubsurface : public BSDF {
 private:
  Lambert lambert;
  float roughness;

 public:
  DisneySubsurface(const std::shared_ptr<Texture<Vec3>>& rho, float roughness)
      : lambert{rho}, roughness{roughness} {}

  Vec3 baseColor(const IntersectInfo& info) const override {
    return lambert.baseColor(info);
  }

  Vec3 bsdf(const Vec3& wo, const IntersectInfo& info,
            const Vec3& wi) const override {
    const Vec3 h = normalize(wo + wi);
    const float cosD = dot(h, wi);
    const float f90 = roughness * cosD * cosD;
    const float cosThetaI = cosTheta(wi);
    const float cosThetaO = cosTheta(wo);

    constexpr float EPS = 1e-9f;
    if (std::abs(cosThetaI * cosThetaO) < EPS) {
      return Vec3(0);
    }

    return lambert.bsdf(wo, info, wi) * 1.25f *
           (schlickFresnelT(wi, f90) * schlickFresnelT(wo, f90) *
                (1.0f / (cosThetaI * cosThetaO) - 0.5f) +
            0.5f);
  }

  Vec3 sample(const Vec3& wo, const IntersectInfo& info, Sampler& sampler,
              Vec3& wi, float& pdf) const {
    wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return bsdf(wo, info, wi);
  }
};

class DisneyBRDF : public BSDF {};

}  // namespace LTRE

#endif