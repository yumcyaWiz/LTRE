#ifndef _LTRE_BXDF_H
#define _LTRE_BXDF_H
#include <algorithm>

#include "LTRE/math/vec2.hpp"
#include "LTRE/math/vec3.hpp"
#include "LTRE/sampling/sampler.hpp"

namespace LTRE {

class Fresnel {
 public:
  virtual float evaluate(float cosThetaI) const = 0;
};

class FresnelDielectric : public Fresnel {
 private:
  float iorI_;
  float iorT_;

 public:
  FresnelDielectric(float iorI, float iorT) : iorI_(iorI), iorT_(iorT) {}

  float evaluate(float cosThetaI) const override {
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
      return 1.0f;
    }

    const float cosThetaT =
        std::sqrt(std::max(0.0f, 1.0f - sinThetaT * sinThetaT));

    float term1 = iorI * cosThetaI;
    float term2 = iorT * cosThetaT;
    const float r_s = (term1 - term2) / (term1 + term2);

    term1 = iorT * cosThetaI;
    term2 = iorI * cosThetaT;
    const float r_p = (term1 - term2) / (term1 + term2);

    return 0.5f * (r_s * r_s + r_p * r_p);
  }
};

class FresnelConductor : public Fresnel {
 private:
  float iorI_;
  float iorT_;
  float extinctionT_;

 public:
  FresnelConductor(float iorI, float iorT, float extinctionT)
      : iorI_(iorI), iorT_(iorT), extinctionT_(extinctionT) {}

  float evaluate(float cosThetaI) const override {
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
    return 0.5 * (Rs + Rp);
  }
};

class FresnelSchlick : public Fresnel {
 private:
  float f0_;

 public:
  FresnelSchlick(float f0) : f0_(f0) {}

  float evaluate(float cosThetaI) const override {
    const auto pow5 = [](float x) { return x * x * x * x * x; };
    return f0_ + (1.0f - f0_) * pow5(1.0f - std::abs(cosThetaI));
  }
};

class BxDF {
 public:
  static float cosTheta(const Vec3& w) { return w[1]; }
  static float cos2Theta(const Vec3& w) { return w[1] * w[1]; }
  static float absCosTheta(const Vec3& w) { return std::abs(w[1]); }

  static float sin2Theta(const Vec3& w) {
    return std::max(0.0f, 1.0f - cos2Theta(w));
  }
  static float sinTheta(const Vec3& w) { return std::sqrt(sin2Theta(w)); }
  static float absSinTheta(const Vec3& w) { return std::abs(sinTheta(w)); }

  static float tanTheta(const Vec3& w) { return sinTheta(w) / cosTheta(w); }
  static float tan2Theta(const Vec3& w) { return sin2Theta(w) / cos2Theta(w); }
  static float absTanTheta(const Vec3& w) { return std::abs(tanTheta(w)); }

  static float cosPhi(const Vec3& w) {
    float sin = sinTheta(w);
    return (sin == 0) ? 1 : std::clamp(w[0] / sin, -1.0f, 1.0f);
  }
  static float sinPhi(const Vec3& w) {
    float sin = sinTheta(w);
    return (sin == 0) ? 0 : std::clamp(w[2] / sin, -1.0f, 1.0f);
  }
  static float cos2Phi(const Vec3& w) { return cosPhi(w) * cosPhi(w); }
  static float sin2Phi(const Vec3& w) { return sinPhi(w) * sinPhi(w); }

  virtual Vec3 f(const Vec3& wo, const Vec3& wi) const = 0;
  virtual Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
                      float& pdf) const = 0;
};

}  // namespace LTRE

#endif