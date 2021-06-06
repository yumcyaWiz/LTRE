#ifndef _INTERSECT_INFO_H
#define _INTERSECT_INFO_H
#include <iostream>

#include "LTRE/math/vec2.hpp"
#include "LTRE/math/vec3.hpp"

namespace LTRE {

class Primitive;

struct IntersectInfo {
  float t;
  Vec3 hitPos;
  Vec3 hitNormal;
  Vec2 uv;
  Vec2 barycentric;
  const Primitive* hitPrimitive;
};

inline std::ostream& operator<<(std::ostream& stream,
                                const IntersectInfo& info) {
  stream << "t: " << info.t << std::endl;
  stream << "hitPos: " << info.hitPos << std::endl;
  stream << "hitNormal: " << info.hitNormal << std::endl;
  stream << "uv: " << info.uv << std::endl;
  stream << "barycentric: " << info.barycentric << std::endl;
  return stream;
}

}  // namespace LTRE

#endif