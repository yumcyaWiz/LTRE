#ifndef _LTRE_DISNEY_H
#define _LTRE_DISNEY_H
#include "LTRE/bsdf/bxdf/bxdf.hpp"
#include "LTRE/bsdf/bxdf/diffuse.hpp"

namespace LTRE {

class DisneyDiffuse : public BxDF {
 private:
  Lambert lambert;

 public:
  DisneyDiffuse() {}

  Vec3 f(const BxDFArgs& args) const override {
    const Vec3 h = normalize(args.wo + args.wi);
    const float cosD = dot(h, args.wi);
    const float f90 = 0.5f + 2.0f * args.roughness * cosD * cosD;
    return lambert.f(args) * schlickFresnelT(args.wi, f90) *
           schlickFresnelT(args.wo, f90);
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
           (schlickFresnelT(args.wi, f90) * schlickFresnelT(args.wo, f90) *
                (1.0f / (cosThetaI * cosThetaO) - 0.5f) +
            0.5f);
  }

  Vec3 sample(Sampler& sampler, BxDFArgs& args, float& pdf) const {
    args.wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return f(args);
  }
};

}  // namespace LTRE

#endif