#ifndef _LTRE_DISNEY_H
#define _LTRE_DISNEY_H
#include "LTRE/bsdf/bxdf/bxdf.hpp"
#include "LTRE/bsdf/bxdf/diffuse.hpp"
#include "LTRE/core/color.hpp"

namespace LTRE {

inline float schlickFresnelR(const Vec3& w, float f0) {
  constexpr auto pow5 = [](float x) { return x * x * x * x * x; };
  return f0 + (1.0f - f0) * pow5(std::max(1.0f - BxDF::absCosTheta(w), 0.0f));
}

inline float schlickFresnelT(const Vec3& w, const Vec3& h, float f90) {
  constexpr auto pow5 = [](float x) { return x * x * x * x * x; };
  return 1.0f + (f90 - 1.0f) * pow5(std::max(1.0f - std::abs(dot(w, h)), 0.0f));
}

class DisneyDiffuse : public BxDF {
 private:
  const Lambert lambert;
  const float roughness;

 public:
  DisneyDiffuse(const Vec3& baseColor, float roughness)
      : lambert{baseColor}, roughness{roughness} {}

  Vec3 f(const Vec3& wo, const Vec3& wi) const override {
    // compute half-vector
    Vec3 wh = wo + wi;
    if (wh[0] == 0 && wh[1] == 0 && wh[2] == 0) return Vec3(0);
    wh = normalize(wh);

    const float cosD = std::abs(dot(wh, wi));
    const float f90 = 0.5f + 2.0f * roughness * cosD * cosD;

    return lambert.f(wo, wi) * schlickFresnelT(wi, wh, f90) *
           schlickFresnelT(wo, wh, f90);
  }

  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi, float& pdf) const {
    wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return f(wo, wi);
  }
};

class DisneySubsurface : public BxDF {
 private:
  const Lambert lambert;
  const float roughness;

 public:
  DisneySubsurface(const Vec3& baseColor, float roughness)
      : lambert{baseColor}, roughness(roughness) {}

  Vec3 f(const Vec3& wo, const Vec3& wi) const override {
    const float cosThetaI = cosTheta(wi);
    const float cosThetaO = cosTheta(wo);
    if (cosThetaI == 0 || cosThetaO == 0) return Vec3(0);

    // compute half-vector
    Vec3 wh = wo + wi;
    if (wh[0] == 0 && wh[1] == 0 && wh[2] == 0) return Vec3(0);
    wh = normalize(wh);

    const float cosD = std::abs(dot(wh, wi));
    const float f90 = roughness * cosD * cosD;

    return lambert.f(wo, wi) * 1.25f *
           (schlickFresnelT(wi, wh, f90) * schlickFresnelT(wo, wh, f90) *
                (1.0f / (cosThetaI * cosThetaO) - 0.5f) +
            0.5f);
  }

  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi, float& pdf) const {
    wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return f(wo, wi);
  }
};

class DisneySheen : public BxDF {
 public:
  const Vec3 baseColor;
  const float sheen;
  const float sheenTint;
  Vec3 rho_tint;
  Vec3 rho_sheen;

  DisneySheen(const Vec3& baseColor, float sheen, float sheenTint)
      : baseColor{baseColor}, sheen(sheen), sheenTint(sheenTint) {
    const float luminance = RGBToXYZ(baseColor)[1];
    if (luminance > 0) {
      rho_tint = baseColor / luminance;
    }
    rho_sheen = lerp(Vec3(1), rho_tint, sheenTint);
  }

  Vec3 f(const Vec3& wo, const Vec3& wi) const override {
    // compute half-vector
    Vec3 wh = wo + wi;
    if (wh[0] == 0 && wh[1] == 0 && wh[2] == 0) return Vec3(0);
    wh = normalize(wh);

    constexpr auto pow5 = [](float x) { return x * x * x * x * x; };
    return sheen * rho_sheen * pow5(std::max(1.0f - dot(wi, wh), 0.0f));
  }

  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi, float& pdf) const {
    wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return f(wo, wi);
  }
};

}  // namespace LTRE

#endif