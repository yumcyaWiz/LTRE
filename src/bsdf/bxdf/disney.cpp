#include "LTRE/bsdf/bxdf/disney.hpp"

#include "LTRE/core/constant.hpp"
#include "LTRE/core/spectrum.hpp"
#include "LTRE/sampling/sampling.hpp"

namespace LTRE {

DisneyDiffuse::DisneyDiffuse(const Vec3& baseColor, float roughness)
    : baseColor{baseColor}, roughness{roughness} {}

Vec3 DisneyDiffuse::f(const Vec3& wo, const Vec3& wi) const {
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
  return baseColor * PI_INV * schlickFresnelT(absCosTheta(wi), f90) *
         schlickFresnelT(absCosTheta(wo), f90);
}

Vec3 DisneyDiffuse::sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
                           float& pdf) const {
  wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
  return f(wo, wi);
}

float DisneyDiffuse::pdf([[maybe_unused]] const Vec3& wo,
                         const Vec3& wi) const {
  return sampleCosineHemispherePdf(wi);
}

float DisneyDiffuse::reflectance(const Vec3& wo) const {
  const float luminance = Spectrum::RGB2XYZ(baseColor)[1];
  return luminance;
}

DisneySubsurface::DisneySubsurface(const Vec3& baseColor, float roughness)
    : baseColor{baseColor}, roughness{roughness} {}

Vec3 DisneySubsurface::f(const Vec3& wo, const Vec3& wi) const {
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
  return baseColor * PI_INV * 1.25f *
         (schlickFresnelT(absCosTheta(wi), f90) *
              schlickFresnelT(absCosTheta(wo), f90) *
              (1.0f / (cosThetaI + cosThetaO) - 0.5f) +
          0.5f);
}

Vec3 DisneySubsurface::sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
                              float& pdf) const {
  wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
  return f(wo, wi);
}

float DisneySubsurface::pdf([[maybe_unused]] const Vec3& wo,
                            const Vec3& wi) const {
  return sampleCosineHemispherePdf(wi);
}

float DisneySubsurface::reflectance(const Vec3& wo) const {
  const float luminance = Spectrum::RGB2XYZ(baseColor)[1];
  return luminance;
}

DisneySheen::DisneySheen(const Vec3& baseColor, float sheen, float sheenTint)
    : baseColor{baseColor}, sheen(sheen), sheenTint(sheenTint) {
  const float luminance =
      0.3 * baseColor[0] + 0.6 * baseColor[1] + 0.1 * baseColor[2];
  if (luminance > 0) {
    rho_tint = baseColor / luminance;
  }
  rho_sheen = lerp(Vec3(1), rho_tint, sheenTint);
}

Vec3 DisneySheen::f(const Vec3& wo, const Vec3& wi) const {
  // compute half-vector
  Vec3 wh = wo + wi;
  if (wh[0] == 0 && wh[1] == 0 && wh[2] == 0) return Vec3(0);
  wh = normalize(wh);

  constexpr auto pow5 = [](float x) { return x * x * x * x * x; };
  return sheen * rho_sheen * pow5(std::max(1.0f - dot(wi, wh), 0.0f));
}

// TODO: more nice sampling
Vec3 DisneySheen::sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
                         float& pdf) const {
  wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
  return f(wo, wi);
}

float DisneySheen::pdf([[maybe_unused]] const Vec3& wo, const Vec3& wi) const {
  return sampleCosineHemispherePdf(wi);
}

float DisneySheen::reflectance(const Vec3& wo) const {
  const float luminance = Spectrum::RGB2XYZ(rho_sheen)[1];
  return sheen * luminance;
}

float DisneySpecular::D(const Vec3& wh) const {
  const float tan2Theta = BxDF::tan2Theta(wh);
  if (std::isinf(tan2Theta)) return 0;
  const float cos4Theta = BxDF::cos2Theta(wh) * BxDF::cos2Theta(wh);

  const float term = 1.0f + tan2Theta / (alpha * alpha);
  return 1.0f / ((PI * alpha * alpha * cos4Theta) * term * term);
}

Vec3 DisneySpecular::sample(const Vec2& uv, float& pdf) const {
  const float theta = std::atan((alpha * std::sqrt(std::max(uv[0], 0.0f))) /
                                std::sqrt(std::max(1.0f - uv[0], 0.0f)));
  const float phi = 2.0f * PI * uv[1];
  const Vec3 wh = sphericalToCartesian(theta, phi);
  pdf = D(wh) * BxDF::absCosTheta(wh);
  return wh;
}

float DisneySpecular::Lambda(const Vec3& w) const {
  const float absTanTheta = BxDF::absTanTheta(w);
  if (std::isinf(absTanTheta)) return 0;

  const float alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);
  return 0.5f * (-1.0f + std::sqrt(1.0f + alpha2Tan2Theta));
}

float DisneySpecular::G1(const Vec3& w) const {
  return 1.0f / (1.0f + Lambda(w));
}

float DisneySpecular::G2(const Vec3& wo, const Vec3& wi) const {
  return G1(wo) * G1(wi);
}

DisneySpecular::DisneySpecular(const Vec3& baseColor, float roughness,
                               float specular, float specularTint,
                               float metallic, float anisotropic) {
  Vec3 rho_tint;
  const float luminance =
      0.3 * baseColor[0] + 0.6 * baseColor[1] + 0.1 * baseColor[2];
  if (luminance > 0) {
    rho_tint = baseColor / luminance;
  }

  const Vec3 rho_specular = lerp(Vec3(1), rho_tint, specularTint);
  const Vec3 f0 = lerp(0.08 * specular * rho_specular, baseColor, metallic);

  // TODO: handle anisotropic parameter
  // const float aspect = std::sqrt(std::max(1.0f - 0.9f * anisotropic,
  // 0.0f)); const float alphaX = std::max(0.001f, roughness * roughness /
  // aspect); const float alphaY = std::max(0.001f, roughness * roughness *
  // aspect);
  F = FresnelSchlick(f0);
  alpha = std::max(roughness * roughness, 0.001f);
}

Vec3 DisneySpecular::f(const Vec3& wo, const Vec3& wi) const {
  const float cosThetaO = absCosTheta(wo);
  const float cosThetaI = absCosTheta(wi);
  if (cosThetaI == 0 || cosThetaO == 0) return Vec3(0);

  // compute half-vector
  Vec3 wh = wo + wi;
  if (wh[0] == 0 && wh[1] == 0 && wh[2] == 0) return Vec3(0);
  wh = normalize(wh);

  const Vec3 f = F.evaluate(dot(wo, wh));
  const float d = D(wh);
  const float g = G2(wo, wi);
  return f * d * g / (4.0f * cosThetaO * cosThetaI);
}

Vec3 DisneySpecular::sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
                            float& pdf) const {
  // sample half-vector
  float pdf_wh;
  const Vec3 wh = sample(sampler.getNext2D(), pdf_wh);

  // compute indident direction
  wi = BxDF::reflect(wo, wh);

  // convert hald-vector pdf to incident direction pdf
  pdf = pdf_wh / (4.0f * dot(wi, wh));

  return f(wo, wi);
}

float DisneySpecular::pdf(const Vec3& wo, const Vec3& wi) const {
  // compute half-vector
  Vec3 wh = wo + wi;
  if (wh[0] == 0 && wh[1] == 0 && wh[2] == 0) return 0;
  wh = normalize(wh);

  // compute half-vector pdf
  const float pdf_wh = D(wh) * absCosTheta(wh);

  // convert hald-vector pdf to incident direction pdf
  return pdf_wh / (4.0f * dot(wi, wh));
}

// NOTE: assuming half-vector is equal to normal
float DisneySpecular::reflectance(const Vec3& wo) const {
  const Vec3 f = F.evaluate(dot(wo, Vec3(0, 1, 0)));
  float luminance = Spectrum::RGB2XYZ(f)[1];
  return luminance;
}

float DisneyClearcoat::D(const Vec3& wh) const {
  const float alpha2 = alpha_D * alpha_D;
  const float cosThetaH = BxDF::absCosTheta(wh);
  const float cosThetaH2 = cosThetaH * cosThetaH;
  return (alpha2 - 1.0f) / (PI * std::log(alpha2)) * 1.0f /
         (1.0f + (alpha2 - 1.0f) * cosThetaH2);
}

float DisneyClearcoat::Lambda(const Vec3& w) const {
  const float absTanTheta = BxDF::absTanTheta(w);
  if (std::isinf(absTanTheta)) return 0;

  const float alpha2Tan2Theta =
      (alpha_G * absTanTheta) * (alpha_G * absTanTheta);
  return 0.5f * (-1.0f + std::sqrt(1.0f + alpha2Tan2Theta));
}

float DisneyClearcoat::G1(const Vec3& w) const {
  return 1.0f / (1.0f + Lambda(w));
}

float DisneyClearcoat::G2(const Vec3& wo, const Vec3& wi) const {
  return G1(wo) * G1(wi);
}

DisneyClearcoat::DisneyClearcoat(float clearcoat, float clearcoatGloss)
    : clearcoat(clearcoat) {
  F = FresnelSchlick(Vec3(0.04f));
  alpha_D = std::lerp(0.1f, 0.001f, clearcoatGloss);
  alpha_G = 0.25f;
}

Vec3 DisneyClearcoat::f(const Vec3& wo, const Vec3& wi) const {
  const float cosThetaO = absCosTheta(wo);
  const float cosThetaI = absCosTheta(wi);
  if (cosThetaI == 0 || cosThetaO == 0) return Vec3(0);

  // compute half-vector
  Vec3 wh = wo + wi;
  if (wh[0] == 0 && wh[1] == 0 && wh[2] == 0) return Vec3(0);
  wh = normalize(wh);

  const Vec3 f = F.evaluate(dot(wo, wh));
  const float d = D(wh);
  const float g = G2(wo, wi);
  return 0.25f * clearcoat * f * d * g / (4.0f * cosThetaO * cosThetaI);
}

Vec3 DisneyClearcoat::sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
                             float& pdf) const {
  const float alpha2 = alpha_D * alpha_D;

  // sample half-vector
  const Vec2 uv = sampler.getNext2D();
  // NOTE: to prevent NaN, max with EPS
  float cosTheta = std::max(
      std::sqrt(std::max(
          (1.0f - std::pow(alpha2, 1.0f - uv[0])) / (1.0f - alpha2), 0.0f)),
      EPS);
  const float sinTheta = std::sqrt(std::max(1.0f - cosTheta * cosTheta, 0.0f));
  const float phi = 2.0f * PI * uv[1];
  const Vec3 wh =
      Vec3(std::cos(phi) * sinTheta, cosTheta, std::sin(phi) * sinTheta);
  const float pdf_wh = D(wh) * cosTheta;

  // compute indident direction
  wi = BxDF::reflect(wo, wh);

  // convert hald-vector pdf to incident direction pdf
  pdf = pdf_wh / (4.0f * dot(wi, wh));

  return f(wo, wi);
}

float DisneyClearcoat::pdf(const Vec3& wo, const Vec3& wi) const {
  // compute half-vector
  Vec3 wh = wo + wi;
  if (wh[0] == 0 && wh[1] == 0 && wh[2] == 0) return 0;
  wh = normalize(wh);

  // compute half-vector pdf
  const float pdf_wh = D(wh) * absCosTheta(wh);

  // convert hald-vector pdf to incident direction pdf
  return pdf_wh / (4.0f * dot(wi, wh));
}

// NOTE: assuming half-vector is equal to normal
float DisneyClearcoat::reflectance(const Vec3& wo) const {
  const Vec3 f = 0.25f * clearcoat * F.evaluate(dot(wo, Vec3(0, 1, 0)));
  float luminance = Spectrum::RGB2XYZ(f)[1];
  return luminance;
}

}  // namespace LTRE