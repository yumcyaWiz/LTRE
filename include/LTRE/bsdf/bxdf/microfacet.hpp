#ifndef _LTRE_MICROFACET_H
#define _LTRE_MICROFACET_H
#include "LTRE/bsdf/bxdf/bxdf.hpp"
#include "LTRE/core/constant.hpp"
#include "LTRE/math/vec3.hpp"
#include "LTRE/sampling/sampling.hpp"

namespace LTRE {

class MicrofacetDistribution {
 public:
  virtual float D(const Vec3& wh) const = 0;
  virtual float Lambda(const Vec3& w) const = 0;
  virtual Vec3 sample(const Vec2& uv, float& pdf) const = 0;

  float G1(const Vec3& w) const { return 1.0f / (1.0f + Lambda(w)); }

  // height-correlated masking-shadowing
  float G(const Vec3& wo, const Vec3& wi) const {
    return 1.0f / (1.0f + Lambda(wo) + Lambda(wi));
  }
};

class Beckmann : public MicrofacetDistribution {
 private:
  float alpha;

 public:
  Beckmann(float alpha) : alpha(alpha) {}

  float D(const Vec3& wh) const override {
    const float tan2Theta = BxDF::tan2Theta(wh);
    if (std::isinf(tan2Theta)) return 0;
    const float cos4Theta = BxDF::cos2Theta(wh) * BxDF::cos2Theta(wh);

    return std::exp(-tan2Theta / (alpha * alpha)) *
           (PI * alpha * alpha * cos4Theta);
  }

  float Lambda(const Vec3& w) const override {
    const float absTanTheta = BxDF::absTanTheta(w);
    if (std::isinf(absTanTheta)) return 0;

    const float a = 1.0f / (alpha * absTanTheta);
    if (a >= 1.6f) {
      return 0;
    }
    return (1.0f - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
  }

  Vec3 sample(const Vec2& uv, float& pdf) const override {
    const float theta = std::atan(
        std::sqrt(-alpha * alpha * std::log(std::max(1.0f - uv[0], 0.0f))));
    const float phi = 2.0f * PI * uv[1];
    const Vec3 wh = sphericalToCartesian(theta, phi);
    pdf = D(wh) * BxDF::absCosTheta(wh);
    return wh;
  }
};

class GGX : public MicrofacetDistribution {
 private:
  float alpha;

 public:
  GGX() {}
  GGX(float alpha) : alpha(alpha) {}

  float D(const Vec3& wh) const override {
    const float tan2Theta = BxDF::tan2Theta(wh);
    if (std::isinf(tan2Theta)) return 0;
    const float cos4Theta = BxDF::cos2Theta(wh) * BxDF::cos2Theta(wh);

    const float term = 1.0f + tan2Theta / (alpha * alpha);
    return 1.0f / ((PI * alpha * alpha * cos4Theta) * term * term);
  }

  float Lambda(const Vec3& w) const override {
    const float absTanTheta = BxDF::absTanTheta(w);
    if (std::isinf(absTanTheta)) return 0;

    const float alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);
    return 0.5f * (-1.0f + std::sqrt(1.0f + alpha2Tan2Theta));
  }

  Vec3 sample(const Vec2& uv, float& pdf) const override {
    const float theta = std::atan((alpha * std::sqrt(std::max(uv[0], 0.0f))) /
                                  std::sqrt(std::max(1.0f - uv[0], 0.0f)));
    const float phi = 2.0f * PI * uv[1];
    const Vec3 wh = sphericalToCartesian(theta, phi);
    pdf = D(wh) * BxDF::absCosTheta(wh);
    return wh;
  }
};

class MicrofacetBRDF : public BxDF {
 private:
  const Vec3 rho;
  const Fresnel* fresnel;
  const MicrofacetDistribution* distribution;

 public:
  MicrofacetBRDF(const Vec3& rho, const Fresnel* fresnel,
                 const MicrofacetDistribution* distribution)
      : rho(rho), fresnel(fresnel), distribution(distribution) {}

  Vec3 f(const Vec3& wo, const Vec3& wi) const override {
    const float cosThetaO = absCosTheta(wo);
    const float cosThetaI = absCosTheta(wi);
    if (cosThetaI == 0 || cosThetaO == 0) return Vec3(0);

    // compute half-vector
    Vec3 wh = wo + wi;
    if (wh[0] == 0 && wh[1] == 0 && wh[2] == 0) return Vec3(0);
    wh = normalize(wh);

    const float F = fresnel->evaluate(dot(wo, wh));
    const float D = distribution->D(wh);
    const float G = distribution->G(wo, wi);
    return rho * F * D * G / (4.0f * cosThetaO * cosThetaI);
  }

  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
              float& pdf) const override {
    // sample half-vector
    float pdf_wh;
    const Vec3 wh = distribution->sample(sampler.getNext2D(), pdf_wh);

    // compute indident direction
    wi = BxDF::reflect(wo, wh);

    // convert hald-vector pdf to incident direction pdf
    pdf = pdf_wh / (4.0f * dot(wi, wh));

    return f(wo, wi);
  }
};

}  // namespace LTRE

#endif