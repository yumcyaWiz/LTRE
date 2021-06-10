#ifndef _LTRE_COLOR_H
#define _LTRE_COLOR_H
#include "LTRE/math/vec3.hpp"

namespace LTRE {

inline Vec3 RGBToXYZ(const Vec3& rgb) {
  Vec3 ret;
  ret[0] = 0.4124564 * rgb[0] + 0.3575761 * rgb[1] + 0.1804375 * rgb[2];
  ret[1] = 0.2126729 * rgb[0] + 0.7151522 * rgb[1] + 0.0721750 * rgb[2];
  ret[2] = 0.0193339 * rgb[0] + 0.1191920 * rgb[1] + 0.9503041 * rgb[2];
  return ret;
}

}  // namespace LTRE

#endif