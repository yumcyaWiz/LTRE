#ifndef _LTRE_LAMBERT_H
#define _LTRE_LAMBERT_H
#include "LTRE/bsdf/bsdf.hpp"
#include "LTRE/core/texture.hpp"
#include "LTRE/sampling/sampling.hpp"

namespace LTRE {

class Lambert : public BSDF {
 public:
  const std::shared_ptr<Texture<Vec3>> rho;

  Lambert(const std::shared_ptr<Texture<Vec3>>& rho) : rho(rho) {}

  Vec3 bsdf(const IntersectInfo& info,
            [[maybe_unused]] const Vec3& wi) const override {
    return rho->sample(info) / PI;
  }
  Vec3 sample([[maybe_unused]] const Vec3& wo, const IntersectInfo& info,
              Sampler& sampler, Vec3& wi, float& pdf) const {
    wi = sampleCosineHemisphere(sampler.getNext2D(), pdf);
    return bsdf(info, wi);
  }
};

}  // namespace LTRE

#endif