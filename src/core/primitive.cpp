#include "LTRE/core/primitive.hpp"

namespace LTRE {

Primitive::Primitive(const std::shared_ptr<Shape>& shape,
                     const std::shared_ptr<Material>& material,
                     const std::shared_ptr<AreaLight>& areaLight)
    : shape(shape), material(material), areaLight(areaLight) {}

std::shared_ptr<AreaLight> Primitive::getAreaLightPtr() const {
  return areaLight;
}

AABB Primitive::aabb() const { return shape->aabb(); }

bool Primitive::intersect(const Ray& ray, IntersectInfo& info) const {
  if (shape->intersect(ray, info)) {
    info.hitPrimitive = this;
    return true;
  } else {
    return false;
  }
}

bool Primitive::intersectP(const Ray& ray) const {
  return shape->intersectP(ray);
}

Vec3 Primitive::evaluateBSDF(const Vec3& wo, const Vec3& wi,
                             const SurfaceInfo& info) const {
  // compute tangent space basis
  Vec3 t, b;
  orthonormalBasis(info.normal, t, b);

  // world to local transform
  const Vec3 wo_l = worldToLocal(wo, t, info.normal, b);
  const Vec3 wi_l = worldToLocal(wi, t, info.normal, b);

  return this->material->f(wo_l, wi_l, info);
}

Vec3 Primitive::sampleBSDF(const Vec3& wo, const SurfaceInfo& info,
                           Sampler& sampler, Vec3& wi, float& pdf) const {
  // compute tangent space basis
  Vec3 t, b;
  orthonormalBasis(info.normal, t, b);

  // world to local transform
  const Vec3 wo_l = worldToLocal(wo, t, info.normal, b);

  // sample direction in tangent space
  Vec3 wi_l;
  const Vec3 bsdf = this->material->sample(sampler, wo_l, info, wi_l, pdf);

  // local to world transform
  wi = localToWorld(wi_l, t, info.normal, b);

  return bsdf;
}

bool Primitive::hasArealight() const { return areaLight != nullptr; }

Vec3 Primitive::Le(const Vec3& wi, const SurfaceInfo& info) const {
  return areaLight->Le(wi, info);
}

Vec3 Primitive::baseColor(const SurfaceInfo& info) const {
  return material->baseColor(info);
}

}  // namespace LTRE