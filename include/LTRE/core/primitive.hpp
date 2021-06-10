#ifndef _LTRE_PRIMITIVE_H
#define _LTRE_PRIMITIVE_H
#include <memory>

#include "LTRE/core/material.hpp"
#include "LTRE/light/area-light.hpp"
#include "LTRE/shape/shape.hpp"

namespace LTRE {

class Primitive {
 private:
  std::shared_ptr<Shape> shape;
  std::shared_ptr<Material> material;
  std::shared_ptr<AreaLight> areaLight;

 public:
  Primitive(const std::shared_ptr<Shape>& shape,
            const std::shared_ptr<Material>& material,
            const std::shared_ptr<AreaLight>& areaLight = nullptr)
      : shape(shape), material(material), areaLight(areaLight) {}

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
    const Vec3 bsdf = this->material->sample(sampler, wo_l, info, wi_l, pdf);

    // local to world transform
    wi = localToWorld(wi_l, t, info.hitNormal, b);

    return bsdf;
  }

  bool hasArealight() const { return areaLight != nullptr; }

  Vec3 Le(const IntersectInfo& info) const { return areaLight->radiance(info); }

  Vec3 baseColor(const IntersectInfo& info) const {
    return material->baseColor(info);
  }
};

}  // namespace LTRE

#endif