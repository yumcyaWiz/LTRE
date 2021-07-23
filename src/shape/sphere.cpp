#include "LTRE/shape/sphere.hpp"

#include "LTRE/sampling/sampling.hpp"

namespace LTRE {

Sphere::Sphere(const Vec3& center, float radius)
    : center(center), radius(radius) {}

// TODO: set uv
bool Sphere::intersect(const Ray& ray, IntersectInfo& info) const {
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

bool Sphere::intersectP(const Ray& ray) const {
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

  return true;
}

AABB Sphere::aabb() const {
  constexpr float EPS = 1e-8f;
  return AABB(center - Vec3(radius + EPS), center + Vec3(radius + EPS));
}

float Sphere::surfaceArea() const { return PI_MUL_4 * radius * radius; }

// TODO: set uv
SurfaceInfo Sphere::samplePoint(Sampler& sampler, float& pdf) const {
  SurfaceInfo ret;
  const Vec3 p = sampleSphere(sampler.getNext2D(), pdf);
  ret.position = center + radius * p;
  ret.normal = p;
  pdf /= (radius * radius);
  return ret;
}

}  // namespace LTRE