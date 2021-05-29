#ifndef _LTRE_PRIMITIVE_H
#define _LTRE_PRIMITIVE_H
#include <memory>

#include "LTRE/bsdf/bsdf.hpp"
#include "LTRE/shape/shape.hpp"

namespace LTRE {

struct Primitive {
  const std::shared_ptr<Shape> shape;
  const std::shared_ptr<BSDF> bsdf;

  Primitive(const std::shared_ptr<Shape>& shape,
            const std::shared_ptr<BSDF>& bsdf)
      : shape(shape), bsdf(bsdf) {}

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