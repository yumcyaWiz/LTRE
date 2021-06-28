#ifndef _LTRE_UNIFORM_SKY_H
#define _LTRE_UNIFORM_SKY_H
#include "LTRE/light/sky/sky.hpp"
#include "LTRE/sampling/sampling.hpp"

namespace LTRE {
class UniformSky : public Sky {
 private:
  const Vec3 ke;

 public:
  UniformSky(const Vec3& ke) : ke(ke) {}

  Vec3 Le([[maybe_unused]] const Vec3& wi,
          [[maybe_unused]] const SurfaceInfo& info) const override {
    return ke;
  }

  Vec3 sampleDirection([[maybe_unused]] const Vec3& pos, Sampler& sampler,
                       Vec3& dir, [[maybe_unused]] float& distToLight,
                       float& pdf) const override {
    dir = sampleSphere(sampler.getNext2D(), pdf);
    return ke;
  }
};
}  // namespace LTRE

#endif