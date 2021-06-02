#ifndef _LTRE_LINEAR_INTERSECTOR_H
#define _LTRE_LINEAR_INTERSECTOR_H
#include "LTRE/intersector/intersector.hpp"

namespace LTRE {

template <Intersectable T>
class LinearIntersector : public Intersector<T> {
 public:
  LinearIntersector() {}

  bool build() override {
    std::cout << "nPrimitives: " << this->primitives.size() << std::endl;
    return true;
  }

  bool intersect(const Ray& ray, IntersectInfo& info) const override {
    bool hit = false;

    for (const auto& prim : this->primitives) {
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