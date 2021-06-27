#ifndef _LTRE_SPHERE_H
#define _LTRE_SPHERE_H
#include "LTRE/shape/shape.hpp"

namespace LTRE {

class Sphere : public Shape {
 private:
  const Vec3 center;
  const float radius;

 public:
  Sphere(const Vec3& center, float radius) : center(center), radius(radius) {}

  bool intersect(const Ray& ray, IntersectInfo& info) const override {
    const float b = dot(ray.origin - center, ray.direction);
    const float c = length2(ray.origin - center) - radius * radius;
    const float D = b * b - c;
    if (D < 0) return false;

    const float t1 = -b - std::sqrt(D);
    const float t2 = -b + std::sqrt(D);
    float t = t1;
    if (t < ray.tmin || t > ray.tmax) {
      t = t2;
      if (t < ray.tmin || t > ray.tmax) return false;
    }

    info.t = t;
    SurfaceInfo surfaceInfo;
    surfaceInfo.position = ray(t);
    surfaceInfo.normal = normalize(surfaceInfo.position - center);
    info.surfaceInfo = surfaceInfo;
    return true;
  }

  AABB aabb() const override {
    constexpr float EPS = 1e-8f;
    return AABB(center - Vec3(radius + EPS), center + Vec3(radius + EPS));
  }

  Vec3 samplePoint(Sampler& sampler, Vec3& normal, float& pdf) const override {
    const Vec3 p = sampleSphere(sampler.getNext2D(), pdf);
    normal = p;
    pdf /= (radius * radius);
    return center + radius * p;
  }
};

}  // namespace LTRE

#endif