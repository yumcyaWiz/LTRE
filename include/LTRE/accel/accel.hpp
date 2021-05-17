#ifndef _LTRE_ACCEL_H
#define _LTRE_ACCEL_H
#include <vector>

#include "LTRE/core/primitive.hpp"

namespace LTRE {

class Accel {
 protected:
  std::vector<Primitive> primitives;

 public:
  Accel() {}

  void addPrimitive(const Primitive& primitive) {
    primitives.push_back(primitive);
  }

  virtual bool intersect(const Ray& ray, IntersectInfo& info) const = 0;
};

}  // namespace LTRE

#endif