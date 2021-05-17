#ifndef _LTRE_INTERSECTOR_H
#define _LTRE_INTERSECTOR_H
#include <vector>

#include "LTRE/core/primitive.hpp"

namespace LTRE {

class Intersector {
 protected:
  std::vector<Primitive> primitives;

 public:
  Intersector() {}

  void addPrimitive(const Primitive& primitive) {
    primitives.push_back(primitive);
  }

  virtual bool intersect(const Ray& ray, IntersectInfo& info) const = 0;
};

}  // namespace LTRE

#endif