#ifndef _LTRE_UNIFORM_SKY_H
#define _LTRE_UNIFORM_SKY_H
#include <limits>

#include "LTRE/light/light.hpp"
#include "LTRE/sampling/sampling.hpp"

namespace LTRE {
class UniformSky : public Light {
 private:
  const Vec3 ke;

 public:
  UniformSky(const Vec3& ke);

  Vec3 power() const override;

  Vec3 Le(const Vec3& wi, const SurfaceInfo& info) const override;

  Vec3 sampleDirection(const SurfaceInfo& surfInfo, Sampler& sampler, Vec3& dir,
                       float& distToLight, float& pdf) const override;
};
}  // namespace LTRE

#endif