#ifndef _LTRE_LIGHT_H
#define _LTRE_LIGHT_H

#include "LTRE/core/types.hpp"
#include "LTRE/math/vec3.hpp"
#include "LTRE/sampling/sampler.hpp"

namespace LTRE {

class Light {
 public:
  virtual Vec3 radiance(const SurfaceInfo& info) const = 0;

  // return Le
  virtual Vec3 sampleDirection(const Vec3& pos, Sampler& sampler, Vec3& dir,
                               float& distToLight, float& pdf) const = 0;
};

}  // namespace LTRE

#endif