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

  AABB aabb() const { return shape->aabb(); }

  bool intersect(const Ray& ray, IntersectInfo& info) const {
    if (shape->intersect(ray, info)) {
      info.hitPrimitive = this;
      return true;
    } else {
      return false;
    }
  }

  Vec3 sampleBSDF(const Vec3& wo, const IntersectInfo& info, Sampler& sampler,
                  Vec3& wi, float& pdf) const {
    // compute tangent space basis
    Vec3 t, b;
    orthonormalBasis(info.hitNormal, t, b);

    // world to local transform
    const Vec3 wo_l = worldToLocal(wo, t, info.hitNormal, b);

    // sample direction in tangent space
    Vec3 wi_l;
    const Vec3 bsdf = this->bsdf->sample(info, sampler, wi_l, pdf);

    // local to world transform
    wi = localToWorld(wi_l, t, info.hitNormal, b);

    return bsdf;
  }
};

}  // namespace LTRE

#endif