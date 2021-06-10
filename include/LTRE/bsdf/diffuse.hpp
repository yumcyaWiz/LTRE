#ifndef _LTRE_LAMBERT_H
#define _LTRE_LAMBERT_H
#include "LTRE/bsdf/bxdf.hpp"
#include "LTRE/core/texture.hpp"
#include "LTRE/sampling/sampling.hpp"

namespace LTRE {

class Lambert : public BxDF {
 public:
  Lambert() {}

  Vec3 f(const BxDFArgs& args) const override { return args.baseColor / PI; }

  Vec3 sample(Sampler& sampler, BxDFArgs& args, float& pdf) const override {
    args.wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return f(args);
  }
};

class OrenNayer : public BxDF {
 public:
  OrenNayer() {}

  Vec3 f(const BxDFArgs& args) const override {
    // compute params
    const float sigma = args.roughness * PI_DIV_4;
    const float sigma2 = sigma * sigma;
    const float A = 1.0f - sigma2 / (2.0f * (sigma2 + 0.33f));
    const float B = 0.45 * sigma2 / (sigma2 + 0.99f);

    // compute max(0, cos(phi_i - phi_o))
    const float sinPhiI = sinPhi(args.wi);
    const float cosPhiI = cosPhi(args.wi);
    const float sinPhiO = sinPhi(args.wo);
    const float cosPhiO = cosPhi(args.wo);
    const float maxCos = std::max(0.0f, cosPhiI * cosPhiO + sinPhiI * sinPhiO);

    // compute sin(alpha), tan(beta)
    const float sinThetaI = sinTheta(args.wi);
    const float sinThetaO = sinTheta(args.wo);
    float sinAlpha, tanBeta;
    if (absCosTheta(args.wi) > absCosTheta(args.wo)) {
      sinAlpha = sinThetaO;
      tanBeta = sinThetaI / absCosTheta(args.wi);
    } else {
      sinAlpha = sinThetaI;
      tanBeta = sinThetaO / absCosTheta(args.wo);
    }

    return args.baseColor * PI_INV * (A + B * maxCos * sinAlpha * tanBeta);
  }

  Vec3 sample(Sampler& sampler, BxDFArgs& args, float& pdf) const override {
    args.wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return f(args);
  }
};

}  // namespace LTRE

#endif