#ifndef _LTRE_LINEAR_INTERSECTOR_H
#define _LTRE_LINEAR_INTERSECTOR_H
#include "LTRE/intersector/intersector.hpp"

namespace LTRE {

class LinearIntersector : public Intersector {
  LinearIntersector() {}

  bool intersect(const Ray& ray, IntersectInfo& info) const override {
    bool hit = false;

    for (const auto& prim : primitives) {
      IntersectInfo temp;
      if (prim.intersect(ray, temp)) {
        ray.tmax = temp.t;
        hit = true;
        info = temp;
      }
    }

    return hit;
  }
};

}  // namespace LTRE

#endif