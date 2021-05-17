#ifndef _LTRE_LINEAR_H
#define _LTRE_LINEAR_H
#include "LTRE/accel/accel.hpp"

namespace LTRE {

class Linear : public Accel {
  Linear() {}

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