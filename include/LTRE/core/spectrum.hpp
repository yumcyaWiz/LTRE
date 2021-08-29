#ifndef _LTRE_SPECTRUM_H
#define _LTRE_SPECTRUM_H
#include "LTRE/core/math/vec3.hpp"

namespace LTRE {

class Spectrum {
  static Vec3 RGB2XYZ(const Vec3& rgb) const;
};

}  // namespace LTRE

#endif