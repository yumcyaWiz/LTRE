#ifndef _LTRE_PRIMITIVE_H
#define _LTRE_PRIMITIVE_H
#include <memory>

#include "LTRE/shape/shape.hpp"

namespace LTRE {

struct Primitive {
  std::shared_ptr<Shape> shape;

  bool intersect(const Ray& ray, IntersectInfo& info) const {
    if (shape->intersect(ray, info)) {
      info.hitPrimitive = this;
      return true;
    } else {
      return false;
    }
  }
};

}  // namespace LTRE

#endif