#include "LTRE/bsdf/bxdf/microfacet.hpp"

#include "LTRE/core/constant.hpp"
#include "LTRE/sampling/sampling.hpp"
#include "spdlog/spdlog.h"

namespace LTRE {

float MicrofacetDistribution::pdf(const Vec3& wh) const {
  return D(wh) * BxDF::absCosTheta(wh);
}

float MicrofacetDistribution::G1(const Vec3& w) const {
  return 1.0f / (1.0f + Lambda(w));
}

float MicrofacetDistribution::G2(const Vec3& wo, const Vec3& wi) const {
  return 1.0f / (1.0f + Lambda(wo) + Lambda(wi));
}

Beckmann::Beckmann(float alpha) : alpha(alpha) {}

float Beckmann::D(const Vec3& wh) const {
  const float tan2Theta = BxDF::tan2Theta(wh);
  if (std::isinf(tan2Theta)) return 0;
  const float cos4Theta = BxDF::cos2Theta(wh) * BxDF::cos2Theta(wh);

  return std::exp(-tan2Theta / (alpha * alpha)) /
         (PI * alpha * alpha * cos4Theta);
}

float Beckmann::Lambda(const Vec3& w) const {
  const float absTanTheta = BxDF::absTanTheta(w);
  if (std::isinf(absTanTheta)) return 0;

  const float a = 1.0f / (alpha * absTanTheta);
  if (a >= 1.6f) {
    return 0;
  }
  return (1.0f - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
}

Vec3 Beckmann::sample(const Vec2& uv, float& pdf) const {
  // NOTE: to prevent NaN, max with EPS
  const float theta = std::atan(
      std::sqrt(-alpha * alpha * std::log(std::max(1.0f - uv[0], EPS))));
  const float phi = 2.0f * PI * uv[1];
  const Vec3 wh = sphericalToCartesian(theta, phi);
  pdf = D(wh) * BxDF::absCosTheta(wh);
  return wh;
}

Berry::Berry() {}

Berry::Berry(float alpha) : alpha(alpha) {}

float Berry::D(const Vec3& wh) const {
  const float alpha2 = alpha * alpha;
  const float cosThetaH = BxDF::absCosTheta(wh);
  const float cosThetaH2 = cosThetaH * cosThetaH;
  return (alpha2 - 1.0f) / (PI * std::log(alpha2)) * 1.0f /
         (1.0f + (alpha2 - 1.0f) * cosThetaH2);
}

// NOTE: use GGX Lambda since Berry has no analytic G, this is inacurate
// though
float Berry::Lambda(const Vec3& w) const {
  const float absTanTheta = BxDF::absTanTheta(w);
  if (std::isinf(absTanTheta)) return 0;

  const float alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);
  return 0.5f * (-1.0f + std::sqrt(1.0f + alpha2Tan2Theta));
}

Vec3 Berry::sample(const Vec2& uv, float& pdf) const {
  const float alpha2 = alpha * alpha;
  // NOTE: to prevent NaN, max with EPS
  float cosTheta = std::max(
      std::sqrt(std::max(
          (1.0f - std::pow(alpha2, 1.0f - uv[0])) / (1.0f - alpha2), 0.0f)),
      EPS);
  const float sinTheta = std::sqrt(std::max(1.0f - cosTheta * cosTheta, 0.0f));
  const float phi = 2.0f * PI * uv[1];
  const Vec3 wh =
      Vec3(std::cos(phi) * sinTheta, cosTheta, std::sin(phi) * sinTheta);
  pdf = D(wh) * cosTheta;
  return wh;
}

GGX::GGX() {}

GGX::GGX(float alpha) : alpha(alpha) {}

float GGX::D(const Vec3& wh) const {
  const float tan2Theta = BxDF::tan2Theta(wh);
  if (std::isinf(tan2Theta)) return 0;
  const float cos4Theta = BxDF::cos2Theta(wh) * BxDF::cos2Theta(wh);

  const float term = 1.0f + tan2Theta / (alpha * alpha);
  return 1.0f / ((PI * alpha * alpha * cos4Theta) * term * term);
}

float GGX::Lambda(const Vec3& w) const {
  const float absTanTheta = BxDF::absTanTheta(w);
  if (std::isinf(absTanTheta)) return 0;

  const float alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);
  return 0.5f * (-1.0f + std::sqrt(1.0f + alpha2Tan2Theta));
}

Vec3 GGX::sample(const Vec2& uv, float& pdf) const {
  const float theta = std::atan((alpha * std::sqrt(std::max(uv[0], 0.0f))) /
                                std::sqrt(std::max(1.0f - uv[0], 0.0f)));
  const float phi = 2.0f * PI * uv[1];
  const Vec3 wh = sphericalToCartesian(theta, phi);
  pdf = D(wh) * BxDF::absCosTheta(wh);
  return wh;
}

MicrofacetBRDF::MicrofacetBRDF() : fresnel(nullptr), distribution(nullptr) {}

MicrofacetBRDF::MicrofacetBRDF(const Fresnel* fresnel,
                               const MicrofacetDistribution* distribution)
    : fresnel(fresnel), distribution(distribution) {}

Vec3 MicrofacetBRDF::f(const Vec3& wo, const Vec3& wi) const {
  // if wo, wi not lie on the same side, return 0
  if (cosTheta(wo) * cosTheta(wi) < 0) {
    return Vec3(0);
  }

  const float cosThetaO = absCosTheta(wo);
  const float cosThetaI = absCosTheta(wi);
  if (cosThetaI == 0 || cosThetaO == 0) return Vec3(0);

  // compute half-vector
  Vec3 wh = wo + wi;
  if (wh[0] == 0 && wh[1] == 0 && wh[2] == 0) return Vec3(0);
  wh = normalize(wh);

  const int sign = cosTheta(wo) > 0 ? 1 : -1;
  const Vec3 F = fresnel->evaluate(sign * std::abs(dot(wo, wh)));
  const float D = distribution->D(wh);
  const float G = distribution->G2(wo, wi);
  return F * D * G / (4.0f * cosThetaO * cosThetaI);
}

Vec3 MicrofacetBRDF::sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
                            float& pdf) const {
  // sample half-vector
  float pdf_wh;
  const Vec3 wh = distribution->sample(sampler.getNext2D(), pdf_wh);

  // compute indident direction
  wi = BxDF::reflect(wo, wh);

  // convert hald-vector pdf to incident direction pdf
  pdf = pdf_wh / (4.0f * dot(wi, wh));

  return f(wo, wi);
}

float MicrofacetBRDF::pdf(const Vec3& wo, const Vec3& wi) const {
  // if wo, wi not lie on the same side, return 0
  if (cosTheta(wo) * cosTheta(wi) < 0) {
    return 0;
  }

  // compute half-vector
  Vec3 wh = wo + wi;
  if (wh[0] == 0 && wh[1] == 0 && wh[2] == 0) return 0;
  wh = normalize(wh);

  // compute half-vector pdf
  const float pdf_wh = distribution->pdf(wh);

  // convert hald-vector pdf to incident direction pdf
  return pdf_wh / (4.0f * dot(wi, wh));
}

MicrofacetBTDF::MicrofacetBTDF() : fresnel(nullptr), distribution(nullptr) {}

MicrofacetBTDF::MicrofacetBTDF(const Fresnel* fresnel,
                               const MicrofacetDistribution* distribution)
    : fresnel(fresnel), distribution(distribution) {}

Vec3 MicrofacetBTDF::f(const Vec3& wo, const Vec3& wi) const {
  // if wo, wi lie on the same side, return 0
  if (cosTheta(wo) * cosTheta(wi) > 0) {
    return Vec3(0);
  }

  const float cosThetaO = absCosTheta(wo);
  const float cosThetaI = absCosTheta(wi);
  if (cosThetaI == 0 || cosThetaO == 0) return Vec3(0);

  const float iorI = fresnel->getIOR_I(cosTheta(wo));
  const float iorT = fresnel->getIOR_T(cosTheta(wo));

  // compute half-vector
  Vec3 wh = -(iorI * wo + iorT * wi);
  if (wh[0] == 0 && wh[1] == 0 && wh[2] == 0) return Vec3(0);
  wh = normalize(wh);

  const float cosThetaOH = dot(wo, wh);
  const float cosThetaIH = dot(wi, wh);
  const int sign = cosTheta(wo) > 0 ? 1 : -1;
  const Vec3 F = fresnel->evaluate(sign * std::abs(cosThetaOH));
  const float D = distribution->D(wh);
  const float G = distribution->G2(wo, wi);
  const float term = iorI * cosThetaOH + iorT * cosThetaIH;
  return std::abs(cosThetaIH) * std::abs(cosThetaOH) * iorT * iorT *
         (1.0f - F) * G * D / (cosThetaI * cosThetaO * term * term);
}

Vec3 MicrofacetBTDF::sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
                            float& pdf) const {
  // sample half-vector
  float pdf_wh;
  Vec3 wh = distribution->sample(sampler.getNext2D(), pdf_wh);

  // flip half-vector if inside object
  const float cosThetaO = cosTheta(wo);
  if (cosThetaO < 0) {
    wh = -wh;
  }

  // compute indident direction
  const float iorI = fresnel->getIOR_I(cosThetaO);
  const float iorT = fresnel->getIOR_T(cosThetaO);
  if (!BxDF::refract(wo, wh, iorI, iorT, wi)) {
    // total reflection
    pdf = 1;
    return Vec3(0);
  }

  // convert half-vector pdf to incident direction pdf
  const float cosThetaIH = dot(wi, wh);
  const float cosThetaOH = dot(wo, wh);
  const float term = iorI * cosThetaOH + iorT * cosThetaIH;
  pdf = pdf_wh * iorT * iorT * std::abs(cosThetaIH) / (term * term);

  return f(wo, wi);
}

float MicrofacetBTDF::pdf(const Vec3& wo, const Vec3& wi) const {
  // if wo, wi lie on the same side, return 0
  if (cosTheta(wo) * cosTheta(wi) > 0) {
    return 0;
  }

  const float iorI = fresnel->getIOR_I(cosTheta(wo));
  const float iorT = fresnel->getIOR_T(cosTheta(wo));

  // compute half-vector
  Vec3 wh = -(iorI * wo + iorT * wi);
  if (wh[0] == 0 && wh[1] == 0 && wh[2] == 0) return 0;
  wh = normalize(wh);

  // compute half-vector pdf
  const float pdf_wh = distribution->pdf(wh);

  // convert hald-vector pdf to incident direction pdf
  const float cosThetaIH = dot(wi, wh);
  const float cosThetaOH = dot(wo, wh);
  const float term = iorI * cosThetaOH + iorT * cosThetaIH;
  return pdf_wh * iorT * iorT * std::abs(cosThetaIH) / (term * term);
}

}  // namespace LTRE
