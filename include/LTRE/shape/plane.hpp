#ifndef _LTRE_PLANE_H
#define _LTRE_PLANE_H
#include "LTRE/sampling/sampler.hpp"
#include "LTRE/shape/shape.hpp"

namespace LTRE {

class Plane : public Shape {
 public:
  const Vec3 leftCornerPoint;
  const Vec3 right;
  const Vec3 up;

  const Vec3 normal;
  const Vec3 center;
  const Vec3 rightDir;
  const float rightLength;
  const Vec3 upDir;
  const float upLength;

  Plane(const Vec3& leftCornerPoint, const Vec3& right, const Vec3& up);

  bool intersect(const Ray& ray, IntersectInfo& info) const override;
  bool intersectP(const Ray& ray) const override;
  AABB aabb() const override;
  float surfaceArea() const override;
  SurfaceInfo samplePoint(Sampler& sampler, float& pdf) const override;
};

}  // namespace LTRE

#endif