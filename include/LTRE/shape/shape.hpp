#ifndef _LTRE_SHAPE_H
#define _LTRE_SHAPE_H
#include "LTRE/core/aabb.hpp"
#include "LTRE/core/ray.hpp"
#include "LTRE/core/types.hpp"

namespace LTRE {

class Shape {
 public:
  virtual bool intersect(const Ray& ray, IntersectInfo& info) const = 0;
  virtual AABB aabb() const = 0;
  virtual Vec3 samplePoint(Sampler& sampler, Vec3& normal,
                           float& pdf) const = 0;
};

}  // namespace LTRE

#endif