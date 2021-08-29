#ifndef _LTRE_SPECTRUM_H
#define _LTRE_SPECTRUM_H
#include "LTRE/math/vec3.hpp"

namespace LTRE {

class Spectrum {
 public:
  static Vec3 RGB2XYZ(const Vec3& rgb);
};

}  // namespace LTRE

#endif