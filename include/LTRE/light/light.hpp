#ifndef _LTRE_LIGHT_H
#define _LTRE_LIGHT_H

#include "LTRE/core/intersect-info.hpp"
#include "LTRE/math/vec3.hpp"
#include "LTRE/sampling/sampler.hpp"

namespace LTRE {

class Light {
 public:
  virtual Vec3 radiance(const IntersectInfo& info) const = 0;
  virtual Vec3 sampleDirection(const IntersectInfo& info, Sampler& sampler,
                               float& distToLight, float& pdf) const = 0;
};

}  // namespace LTRE

#endif