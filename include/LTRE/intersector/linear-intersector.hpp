#ifndef _LTRE_LINEAR_INTERSECTOR_H
#define _LTRE_LINEAR_INTERSECTOR_H
#include "LTRE/intersector/intersector.hpp"

namespace LTRE {

template <Intersectable T>
class LinearIntersector : public Intersector<T> {
 public:
  LinearIntersector() {}
  LinearIntersector(const std::vector<T>& primitives)
      : Intersector<T>(primitives) {}

  bool build() override {
    std::cout << "nPrimitives: " << this->primitives.size() << std::endl;
    return true;
  }

  AABB aabb() const {
    AABB ret;
    for (const auto& prim : this->primitives) {
      ret = mergeAABB(ret, prim.aabb());
    }
    return ret;
  }

  bool intersect(const Ray& ray, IntersectInfo& info) const override {
    bool hit = false;

    for (const auto& prim : this->primitives) {
      if (prim.intersect(ray, info)) {
        ray.tmax = info.t;
        hit = true;
      }
    }

    return hit;
  }
};

}  // namespace LTRE

#endif