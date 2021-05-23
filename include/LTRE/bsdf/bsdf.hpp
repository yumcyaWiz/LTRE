#ifndef _LTRE_BSDF_H
#define _LTRE_BSDF_H
#include "LTRE/core/intersect-info.hpp"
#include "LTRE/math/vec3.hpp"

namespace LTRE {

class BSDF {
 public:
  virtual Vec3 bsdf(const Vec3& wi, const Vec3& wo) const = 0;
  virtual Vec3 sample(const IntersectInfo& info, float& pdf) const = 0;
};

}  // namespace LTRE

#endif