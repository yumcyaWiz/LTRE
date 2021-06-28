#ifndef _LTRE_LINEAR_INTERSECTOR_H
#define _LTRE_LINEAR_INTERSECTOR_H
#include "LTRE/intersector/intersector.hpp"
#include "spdlog/spdlog.h"

namespace LTRE {

template <Intersectable T>
class LinearIntersector : public Intersector<T> {
 public:
  LinearIntersector() {}
  LinearIntersector(const std::vector<T>& primitives)
      : Intersector<T>(primitives) {}

  bool build() override {
    spdlog::info("[LinearIntersector] nPrimitives: " +
                 std::to_string(this->primitives.size()));
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

  bool intersectP(const Ray& ray) const override {
    for (const auto& prim : this->primitives) {
      if (prim.intersectP(ray)) {
        return true;
      }
    }

    return false;
  }
};

}  // namespace LTRE

#endif