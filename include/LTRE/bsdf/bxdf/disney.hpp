#ifndef _LTRE_DISNEY_H
#define _LTRE_DISNEY_H
#include "LTRE/bsdf/bxdf/bxdf.hpp"
#include "LTRE/bsdf/bxdf/diffuse.hpp"
#include "LTRE/bsdf/bxdf/microfacet.hpp"
#include "LTRE/core/color.hpp"

namespace LTRE {

inline float schlickFresnelR(float cos, float f0) {
  constexpr auto pow5 = [](float x) { return x * x * x * x * x; };
  return f0 + (1.0f - f0) * pow5(std::max(1.0f - cos, 0.0f));
}

inline Vec3 schlickFresnelR(float cos, const Vec3& f0) {
  constexpr auto pow5 = [](float x) { return x * x * x * x * x; };
  return f0 + (1.0f - f0) * pow5(std::max(1.0f - cos, 0.0f));
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

    constexpr auto schlickFresnelT = [](float cos, float f90) {
      constexpr auto pow5 = [](float x) { return x * x * x * x * x; };
      return 1.0f + (f90 - 1.0f) * pow5(std::max(1.0f - cos, 0.0f));
    };
    return lambert.f(wo, wi) * schlickFresnelT(absCosTheta(wi), f90) *
           schlickFresnelT(absCosTheta(wo), f90);
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
    const float cosThetaI = absCosTheta(wi);
    const float cosThetaO = absCosTheta(wo);
    if (cosThetaI == 0 || cosThetaO == 0) {
      return Vec3(0);
    }

    // compute half-vector
    Vec3 wh = wo + wi;
    if (wh[0] == 0 && wh[1] == 0 && wh[2] == 0) {
      return Vec3(0);
    }
    wh = normalize(wh);

    const float cosD = std::abs(dot(wh, wi));
    const float f90 = roughness * cosD * cosD;

    constexpr auto schlickFresnelT = [](float cos, float f90) {
      constexpr auto pow5 = [](float x) { return x * x * x * x * x; };
      return 1.0f + (f90 - 1.0f) * pow5(std::max(1.0f - cos, 0.0f));
    };
    return lambert.f(wo, wi) * 1.25f *
           (schlickFresnelT(absCosTheta(wi), f90) *
                schlickFresnelT(absCosTheta(wo), f90) *
                (1.0f / (cosThetaI + cosThetaO) - 0.5f) +
            0.5f);
  }

  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi, float& pdf) const {
    wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return f(wo, wi);
  }
};

class DisneySheen : public BxDF {
 private:
  const Vec3 baseColor;
  const float sheen;
  const float sheenTint;
  Vec3 rho_tint;
  Vec3 rho_sheen;

 public:
  DisneySheen(const Vec3& baseColor, float sheen, float sheenTint)
      : baseColor{baseColor}, sheen(sheen), sheenTint(sheenTint) {
    const float luminance =
        0.3 * baseColor[0] + 0.6 * baseColor[1] + 0.1 * baseColor[2];
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

class DisneySpecular : public BxDF {
 private:
  GGX distribution;
  Vec3 f0;

 public:
  DisneySpecular(const Vec3& baseColor, float roughness, float specular,
                 float specularTint, float metallic, float anisotropic) {
    Vec3 rho_tint;
    const float luminance =
        0.3 * baseColor[0] + 0.6 * baseColor[1] + 0.1 * baseColor[2];
    if (luminance > 0) {
      rho_tint = baseColor / luminance;
    }

    const Vec3 rho_specular = lerp(Vec3(1), rho_tint, specularTint);
    f0 = lerp(0.08 * specular * rho_specular, baseColor, metallic);

    // TODO: handle anisotropic parameter
    // const float aspect = std::sqrt(std::max(1.0f - 0.9f * anisotropic,
    // 0.0f)); const float alphaX = std::max(0.001f, roughness * roughness /
    // aspect); const float alphaY = std::max(0.001f, roughness * roughness *
    // aspect);
    distribution = GGX(roughness * roughness);
  }

  Vec3 f(const Vec3& wo, const Vec3& wi) const override {
    const float cosThetaO = absCosTheta(wo);
    const float cosThetaI = absCosTheta(wi);
    if (cosThetaI == 0 || cosThetaO == 0) return Vec3(0);

    // compute half-vector
    Vec3 wh = wo + wi;
    if (wh[0] == 0 && wh[1] == 0 && wh[2] == 0) return Vec3(0);
    wh = normalize(wh);

    const Vec3 F = schlickFresnelR(std::abs(dot(wi, wh)), f0);
    const float D = distribution.D(wh);
    const float G = distribution.G(wo, wi);
    return F * D * G / (4.0f * cosThetaO * cosThetaI);
  }

  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
              float& pdf) const override {
    wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return f(wo, wi);
  }
};

}  // namespace LTRE

#endif