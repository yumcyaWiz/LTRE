#ifndef _LTRE_INTERSECTOR_H
#define _LTRE_INTERSECTOR_H
#include <vector>

#include "LTRE/core/primitive.hpp"

namespace LTRE {

template <typename T>
concept Intersectable = requires(T& x, const Ray& ray, IntersectInfo& info) {
  x.intersect(ray, info);
  x.aabb();
};

template <Intersectable T>
class Intersector {
 protected:
  std::vector<T> primitives;

  Intersector() {}
  Intersector(const std::vector<T>& primitives) : primitives(primitives) {}

 public:
  void addPrimitive(const T& primitive) { primitives.push_back(primitive); }

  virtual bool build() = 0;
  virtual bool intersect(const Ray& ray, IntersectInfo& info) const = 0;
};

}  // namespace LTRE

#endif