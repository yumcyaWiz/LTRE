#include "LTRE/bsdf/bxdf/bxdf.hpp"

namespace LTRE {

FresnelDielectric::FresnelDielectric(float iorI, float iorT)
    : iorI_(iorI), iorT_(iorT) {}

Vec3 FresnelDielectric::evaluate(float cosThetaI) const {
  // handle from internal case
  float iorI = iorI_;
  float iorT = iorT_;
  if (cosThetaI < 0) {
    std::swap(iorI, iorT);
    cosThetaI = std::abs(cosThetaI);
  }

  // compute sinThetaT by snell's law
  const float sinThetaI =
      std::sqrt(std::max(0.0f, 1.0f - cosThetaI * cosThetaI));
  const float sinThetaT = iorI / iorT * sinThetaI;

  // total reflection
  if (std::abs(sinThetaT) > 1.0f) {
    return Vec3(1);
  }

  const float cosThetaT =
      std::sqrt(std::max(0.0f, 1.0f - sinThetaT * sinThetaT));

  float term1 = iorI * cosThetaI;
  float term2 = iorT * cosThetaT;
  const float r_s = (term1 - term2) / (term1 + term2);

  term1 = iorT * cosThetaI;
  term2 = iorI * cosThetaT;
  const float r_p = (term1 - term2) / (term1 + term2);

  return 0.5f * (r_s * r_s + r_p * r_p) * Vec3(1);
}

FresnelConductor::FresnelConductor(float iorI, float iorT, float extinctionT)
    : iorI_(iorI), iorT_(iorT), extinctionT_(extinctionT) {}

Vec3 FresnelConductor::evaluate(float cosThetaI) const {
  // handle from internal case
  float iorI = iorI_;
  float iorT = iorT_;
  float extinctionI = 1.0f;
  float extinctionT = extinctionT_;
  if (cosThetaI < 0) {
    std::swap(iorI, iorT);
    std::swap(extinctionI, extinctionT);
    cosThetaI = std::abs(cosThetaI);
  }

  const float cosThetaI2 = cosThetaI * cosThetaI;
  const float sinThetaI2 = std::max(0.0f, 1.0f - cosThetaI2);
  const float sinThetaI4 = sinThetaI2 * sinThetaI2;

  const float eta = iorT / iorI;
  const float eta2 = eta * eta;
  const float eta_k = extinctionT / extinctionI;
  const float eta_k2 = eta_k * eta_k;

  const float term = eta2 - eta_k2 - sinThetaI2;
  const float a2plusb2 = std::sqrt(term * term + 4.0f * eta2 * eta_k2);
  const float a = std::sqrt(0.5f * (a2plusb2 + term));

  float term1 = a2plusb2 + cosThetaI2;
  float term2 = 2 * a * cosThetaI;
  const float Rs = (term1 - term2) / (term1 + term2);
  term1 = cosThetaI2 * a2plusb2 + sinThetaI4;
  term2 = 2 * a * cosThetaI * sinThetaI2;
  const float Rp = Rs * (term1 - term2) / (term1 + term2);
  return 0.5 * (Rs + Rp) * Vec3(1);
}

FresnelSchlick::FresnelSchlick() {}

FresnelSchlick::FresnelSchlick(const Vec3& f0) : f0_(f0) {}

Vec3 FresnelSchlick::evaluate(float cosThetaI) const {
  const auto pow5 = [](float x) { return x * x * x * x * x; };
  return f0_ + (1.0f - f0_) * pow5(1.0f - std::abs(cosThetaI));
}

float BxDF::cosTheta(const Vec3& w) { return w[1]; }
float BxDF::cos2Theta(const Vec3& w) { return w[1] * w[1]; }
float BxDF::absCosTheta(const Vec3& w) { return std::abs(w[1]); }

float BxDF::sin2Theta(const Vec3& w) {
  return std::max(0.0f, 1.0f - cos2Theta(w));
}
float BxDF::sinTheta(const Vec3& w) { return std::sqrt(sin2Theta(w)); }
float BxDF::absSinTheta(const Vec3& w) { return std::abs(sinTheta(w)); }

float BxDF::tanTheta(const Vec3& w) { return sinTheta(w) / cosTheta(w); }
float BxDF::tan2Theta(const Vec3& w) { return sin2Theta(w) / cos2Theta(w); }
float BxDF::absTanTheta(const Vec3& w) { return std::abs(tanTheta(w)); }

float BxDF::cosPhi(const Vec3& w) {
  float sin = sinTheta(w);
  return (sin == 0) ? 1 : std::clamp(w[0] / sin, -1.0f, 1.0f);
}
float BxDF::sinPhi(const Vec3& w) {
  float sin = sinTheta(w);
  return (sin == 0) ? 0 : std::clamp(w[2] / sin, -1.0f, 1.0f);
}
float BxDF::cos2Phi(const Vec3& w) { return cosPhi(w) * cosPhi(w); }
float BxDF::sin2Phi(const Vec3& w) { return sinPhi(w) * sinPhi(w); }

Vec3 BxDF::reflect(const Vec3& v, const Vec3& n) {
  return -v + 2.0f * dot(v, n) * n;
}

}  // namespace LTRE