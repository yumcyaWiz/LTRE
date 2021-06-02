#ifndef _LTRE_BSDF_H
#define _LTRE_BSDF_H
#include "LTRE/core/intersect-info.hpp"
#include "LTRE/math/vec3.hpp"
#include "LTRE/sampling/sampler.hpp"

namespace LTRE {

class BSDF {
 public:
  virtual Vec3 baseColor(const IntersectInfo& info) const = 0;
  virtual Vec3 bsdf(const IntersectInfo& info, const Vec3& wi) const = 0;
  virtual Vec3 sample(const Vec3& wo, const IntersectInfo& info,
                      Sampler& sampler, Vec3& wi, float& pdf) const = 0;
};

}  // namespace LTRE

#endif