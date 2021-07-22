#ifndef _LTRE_AABB_H
#define _LTRE_AABB_H

#include "LTRE/core/ray.hpp"
#include "LTRE/math/vec3.hpp"

namespace LTRE {

struct AABB {
  Vec3 bounds[2];

  explicit AABB();
  explicit AABB(const Vec3& pMin, const Vec3& pMax);

  Vec3 center() const;

  int longestAxis() const;

  float surfaceArea() const;

  bool intersect(const Ray& ray, const Vec3& dirInv,
                 const int dirInvSign[3]) const;
};

AABB mergeAABB(const AABB& bbox, const Vec3& p);
AABB mergeAABB(const AABB& bbox1, const AABB& bbox2);

std::ostream& operator<<(std::ostream& stream, const AABB& bbox);

}  // namespace LTRE

#endif