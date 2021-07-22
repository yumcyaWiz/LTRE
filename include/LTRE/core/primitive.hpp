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
            const std::shared_ptr<AreaLight>& areaLight = nullptr);

  std::shared_ptr<AreaLight> getAreaLightPtr() const;
  AABB aabb() const;

  bool intersect(const Ray& ray, IntersectInfo& info) const;
  bool intersectP(const Ray& ray) const;

  Vec3 evaluateBSDF(const Vec3& wo, const Vec3& wi,
                    const SurfaceInfo& info) const;

  Vec3 sampleBSDF(const Vec3& wo, const SurfaceInfo& info, Sampler& sampler,
                  Vec3& wi, float& pdf) const;

  bool hasArealight() const;

  Vec3 Le(const Vec3& wi, const SurfaceInfo& info) const;

  Vec3 baseColor(const SurfaceInfo& info) const;
};

}  // namespace LTRE

#endif