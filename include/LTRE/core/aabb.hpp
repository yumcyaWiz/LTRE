#ifndef _LTRE_AABB_H
#define _LTRE_AABB_H
#include <limits>

#include "LTRE/math/vec3.hpp"

namespace LTRE {

struct AABB {
  Vec3 bounds[2];

  explicit AABB()
      : bounds{Vec3(std::numeric_limits<float>::max()),
               Vec3(std::numeric_limits<float>::lowest())} {}
  explicit AABB(const Vec3& pMin, const Vec3& pMax) : bounds{pMin, pMax} {}

  Vec3 center() const { return 0.5f * (bounds[0] + bounds[1]); }

  int longestAxis() const {
    const Vec3 length = bounds[1] - bounds[0];
    // x
    if (length[0] >= length[1] && length[0] >= length[2]) {
      return 0;
    }
    // y
    else if (length[1] >= length[0] && length[1] >= length[2]) {
      return 1;
    }
    // z
    else {
      return 2;
    }
  }

  float surfaceArea() const {
    const Vec3 length = bounds[1] - bounds[0];
    float area = 2.0f * length[0] * length[1] + 2.0f * length[0] * length[2] +
                 2.0f * length[1] * length[2];
    // handle null AABB case
    if (std::isinf(area)) area = 0;

    return area;
  }

  bool intersect(const Ray& ray, const Vec3& dirInv,
                 const int dirInvSign[3]) const {
    // https://dl.acm.org/doi/abs/10.1145/1198555.1198748
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    tmin = (bounds[dirInvSign[0]][0] - ray.origin[0]) * dirInv[0];
    tmax = (bounds[1 - dirInvSign[0]][0] - ray.origin[0]) * dirInv[0];
    tymin = (bounds[dirInvSign[1]][1] - ray.origin[1]) * dirInv[1];
    tymax = (bounds[1 - dirInvSign[1]][1] - ray.origin[1]) * dirInv[1];
    if (tmin > tymax || tymin > tmax) return false;
    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    tzmin = (bounds[dirInvSign[2]][2] - ray.origin[2]) * dirInv[2];
    tzmax = (bounds[1 - dirInvSign[2]][2] - ray.origin[2]) * dirInv[2];
    if (tmin > tzmax || tzmin > tmax) return false;
    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    return tmin < ray.tmax && tmax > ray.tmin;
  }
};

inline AABB mergeAABB(const AABB& bbox, const Vec3& p) {
  AABB ret;
  for (int i = 0; i < 3; ++i) {
    ret.bounds[0][i] = std::min(bbox.bounds[0][i], p[i]);
    ret.bounds[1][i] = std::max(bbox.bounds[1][i], p[i]);
  }
  return ret;
}

inline AABB mergeAABB(const AABB& bbox1, const AABB& bbox2) {
  AABB ret;
  for (int i = 0; i < 3; ++i) {
    ret.bounds[0][i] = std::min(bbox1.bounds[0][i], bbox2.bounds[0][i]);
    ret.bounds[1][i] = std::max(bbox1.bounds[1][i], bbox2.bounds[1][i]);
  }
  return ret;
}

inline std::ostream& operator<<(std::ostream& stream, const AABB& bbox) {
  stream << bbox.bounds[0] << ", " << bbox.bounds[1];
  return stream;
}

}  // namespace LTRE

#endif