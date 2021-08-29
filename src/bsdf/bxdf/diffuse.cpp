#include "LTRE/bsdf/bxdf/diffuse.hpp"

#include "LTRE/core/constant.hpp"
#include "LTRE/core/spectrum.hpp"
#include "LTRE/sampling/sampling.hpp"

namespace LTRE {

Lambert::Lambert(const Vec3& rho) : rho(rho) {}

Vec3 Lambert::f([[maybe_unused]] const Vec3& wo,
                [[maybe_unused]] const Vec3& wi) const {
  return rho / PI;
}

Vec3 Lambert::sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
                     float& pdf) const {
  wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
  return f(wo, wi);
}

float Lambert::pdf([[maybe_unused]] const Vec3& wo, const Vec3& wi) const {
  return sampleCosineHemispherePdf(wi);
}

float Lambert::reflectance(const Vec3& wo) const {
  const float luminance = Spectrum::RGB2XYZ(rho)[1];
  return luminance;
}

OrenNayer::OrenNayer(const Vec3& rho, float sigma) : rho(rho), sigma(sigma) {
  // compute params
  const float sigma2 = sigma * sigma;
  A = 1.0f - sigma2 / (2.0f * (sigma2 + 0.33f));
  B = 0.45 * sigma2 / (sigma2 + 0.99f);
}

Vec3 OrenNayer::f(const Vec3& wo, const Vec3& wi) const {
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

Vec3 OrenNayer::sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
                       float& pdf) const {
  wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
  return f(wo, wi);
}

float OrenNayer::pdf([[maybe_unused]] const Vec3& wo, const Vec3& wi) const {
  return sampleCosineHemispherePdf(wi);
}

float OrenNayer::reflectance(const Vec3& wo) const {
  const float luminance = Spectrum::RGB2XYZ(rho)[1];
  return luminance;
}

}  // namespace LTRE