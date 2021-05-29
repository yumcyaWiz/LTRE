#ifndef _LTRE_SHAPE_H
#define _LTRE_SHAPE_H
#include "LTRE/core/aabb.hpp"
#include "LTRE/core/intersect-info.hpp"
#include "LTRE/core/ray.hpp"

namespace LTRE {

class Shape {
 public:
  virtual bool intersect(const Ray& ray, IntersectInfo& info) const = 0;
  virtual AABB aabb() const = 0;
};

}  // namespace LTRE

#endif