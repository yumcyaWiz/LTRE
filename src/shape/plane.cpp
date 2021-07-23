#include "LTRE/shape/plane.hpp"

#include "LTRE/sampling/sampling.hpp"

namespace LTRE {

Plane::Plane(const Vec3& leftCornerPoint, const Vec3& right, const Vec3& up)
    : leftCornerPoint(leftCornerPoint),
      right(right),
      up(up),
      normal(normalize(cross(right, up))),
      center(leftCornerPoint + 0.5f * right + 0.5f * up),
      rightDir(normalize(right)),
      rightLength(length(right)),
      upDir(normalize(up)),
      upLength(length(up)) {}

bool Plane::intersect(const Ray& ray, IntersectInfo& info) const {
  const float t =
      -dot(ray.origin - center, normal) / dot(ray.direction, normal);
  if (t < ray.tmin || t > ray.tmax) return false;

  const Vec3 hitPos = ray(t);
  const float dx = dot(hitPos - leftCornerPoint, rightDir);
  const float dy = dot(hitPos - leftCornerPoint, upDir);
  if (dx < 0.0f || dx > rightLength || dy < 0.0f || dy > upLength) return false;

  info.t = t;
  info.barycentric = Vec2(dx / rightLength, dy / upLength);
  SurfaceInfo surfaceInfo;
  surfaceInfo.position = ray(t);
  surfaceInfo.normal = normal;
  surfaceInfo.uv = info.barycentric;
  info.surfaceInfo = surfaceInfo;
  return true;
}

bool Plane::intersectP(const Ray& ray) const {
  const float t =
      -dot(ray.origin - center, normal) / dot(ray.direction, normal);
  if (t < ray.tmin || t > ray.tmax) return false;

  const Vec3 hitPos = ray(t);
  const float dx = dot(hitPos - leftCornerPoint, rightDir);
  const float dy = dot(hitPos - leftCornerPoint, upDir);
  if (dx < 0.0f || dx > rightLength || dy < 0.0f || dy > upLength) return false;

  return true;
}

AABB Plane::aabb() const {
  return AABB(leftCornerPoint - Vec3(EPS),
              leftCornerPoint + right + up + Vec3(EPS));
}

float Plane::surfaceArea() const { return rightLength * upLength; }

SurfaceInfo Plane::samplePoint(Sampler& sampler, float& pdf) const {
  SurfaceInfo ret;
  const Vec2 p = samplePlane(sampler.getNext2D(), rightLength, upLength, pdf);
  ret.position = leftCornerPoint + p[0] * rightDir + p[1] * upDir;
  ret.normal = normal;
  ret.uv = Vec2(p[0] / rightLength, p[1] / upLength);
  return ret;
}

}  // namespace LTRE