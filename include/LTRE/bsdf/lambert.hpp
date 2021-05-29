#ifndef _LTRE_LAMBERT_H
#define _LTRE_LAMBERT_H
#include "LTRE/bsdf/bsdf.hpp"
#include "LTRE/sampling/sampling.hpp"

namespace LTRE {

class Lambert : public BSDF {
 public:
  const Vec3 rho;

  Lambert(const Vec3& rho) : rho(rho) {}

  Vec3 bsdf([[maybe_unused]] const IntersectInfo& info,
            [[maybe_unused]] const Vec3& wi) const override {
    return rho / PI;
  }
  Vec3 sample([[maybe_unused]] const Vec3& wo, const IntersectInfo& info,
              Sampler& sampler, Vec3& wi, float& pdf) const {
    wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return bsdf(info, wi);
  }
};

}  // namespace LTRE

#endif