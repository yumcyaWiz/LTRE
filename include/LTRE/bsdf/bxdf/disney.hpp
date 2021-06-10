#ifndef _LTRE_DISNEY_H
#define _LTRE_DISNEY_H
#include "LTRE/bsdf/bxdf/bxdf.hpp"
#include "LTRE/bsdf/bxdf/diffuse.hpp"
#include "LTRE/core/color.hpp"

namespace LTRE {

inline float schlickFresnelR(const Vec3& w, float f0) {
  return f0 +
         (1.0f - f0) * std::pow(1.0f - std::max(BxDF::cosTheta(w), 0.0f), 5.0f);
}

inline float schlickFresnelT(const Vec3& w, const Vec3& h, float f90) {
  return 1.0f + (f90 - 1.0f) * std::pow(1.0f - std::max(dot(w, h), 0.0f), 5.0f);
}

class DisneyDiffuse : public BxDF {
 private:
  Lambert lambert;

 public:
  DisneyDiffuse() {}

  Vec3 f(const BxDFArgs& args) const override {
    const Vec3 h = normalize(args.wo + args.wi);

    const float cosD = dot(h, args.wi);
    const float f90 = 0.5f + 2.0f * args.roughness * cosD * cosD;

    return lambert.f(args) * schlickFresnelT(args.wi, h, f90) *
           schlickFresnelT(args.wo, h, f90);
  }

  Vec3 sample(Sampler& sampler, BxDFArgs& args, float& pdf) const {
    args.wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return f(args);
  }
};

class DisneySubsurface : public BxDF {
 private:
  Lambert lambert;

 public:
  DisneySubsurface() {}

  Vec3 f(const BxDFArgs& args) const override {
    const Vec3 h = normalize(args.wo + args.wi);

    const float cosD = dot(h, args.wi);
    const float f90 = args.roughness * cosD * cosD;
    const float cosThetaI = cosTheta(args.wi);
    const float cosThetaO = cosTheta(args.wo);

    constexpr float EPS = 1e-9f;
    if (std::abs(cosThetaI * cosThetaO) < EPS) {
      return Vec3(0);
    }

    return lambert.f(args) * 1.25f *
           (schlickFresnelT(args.wi, h, f90) *
                schlickFresnelT(args.wo, h, f90) *
                (1.0f / (cosThetaI * cosThetaO) - 0.5f) +
            0.5f);
  }

  Vec3 sample(Sampler& sampler, BxDFArgs& args, float& pdf) const {
    args.wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return f(args);
  }
};

class DisneySheen : public BxDF {
 public:
  DisneySheen() {}

  Vec3 f(const BxDFArgs& args) const override {
    const Vec3 h = normalize(args.wi + args.wo);

    const float luminance = RGBToXYZ(args.baseColor)[1];
    Vec3 rho_tint;
    if (luminance > 0) {
      rho_tint = args.baseColor / luminance;
    }
    const Vec3 rho_sheen = lerp(Vec3(1), rho_tint, args.sheenTint);

    return args.sheen * rho_sheen *
           std::pow(std::max(1.0f - dot(args.wi, h), 0.0f), 5.0f);
  }

  Vec3 sample(Sampler& sampler, BxDFArgs& args, float& pdf) const {
    args.wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return f(args);
  }
};

}  // namespace LTRE

#endif