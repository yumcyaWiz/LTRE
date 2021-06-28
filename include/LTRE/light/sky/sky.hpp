#ifndef _LTRE_SKY_H
#define _LTRE_SKY_H
#include "LTRE/light/light.hpp"
#include "LTRE/math/vec3.hpp"

namespace LTRE {

class Sky : public Light {
 public:
  virtual Vec3 Le(const Vec3& wi, const SurfaceInfo& info) const = 0;

  virtual Vec3 sampleDirection(const Vec3& pos, Sampler& sampler, Vec3& dir,
                               float& distToLight, float& pdf) const = 0;
};

}  // namespace LTRE

#endif