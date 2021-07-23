#ifndef _LTRE_SPHERE_H
#define _LTRE_SPHERE_H
#include "LTRE/sampling/sampler.hpp"
#include "LTRE/shape/shape.hpp"

namespace LTRE {

class Sphere : public Shape {
 private:
  const Vec3 center;
  const float radius;

 public:
  Sphere(const Vec3& center, float radius);

  bool intersect(const Ray& ray, IntersectInfo& info) const override;
  bool intersectP(const Ray& ray) const override;
  AABB aabb() const override;
  float surfaceArea() const override;
  SurfaceInfo samplePoint(Sampler& sampler, float& pdf) const override;
};

}  // namespace LTRE

#endif