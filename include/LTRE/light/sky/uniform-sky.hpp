#ifndef _LTRE_UNIFORM_SKY_H
#define _LTRE_UNIFORM_SKY_H
#include <limits>

#include "LTRE/light/sky/sky.hpp"
#include "LTRE/sampling/sampling.hpp"

namespace LTRE {
class UniformSky : public Sky {
 private:
  const Vec3 ke;

 public:
  UniformSky(const Vec3& ke) : ke(ke) {}

  Vec3 power() const override { return PI_MUL_4 * 10000.0f * ke; }

  Vec3 Le([[maybe_unused]] const Vec3& wi,
          [[maybe_unused]] const SurfaceInfo& info) const override {
    return ke;
  }

  Vec3 sampleDirection([[maybe_unused]] const Vec3& pos, Sampler& sampler,
                       Vec3& dir, float& distToLight,
                       float& pdf) const override {
    dir = sampleSphere(sampler.getNext2D(), pdf);
    distToLight = std::numeric_limits<float>::max();
    return ke;
  }
};
}  // namespace LTRE

#endif