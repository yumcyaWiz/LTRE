#ifndef _LTRE_SKY_H
#define _LTRE_SKY_H
#include "LTRE/math/vec3.hpp"

namespace LTRE {

class Sky {
 public:
  virtual Vec3 radiance(const Ray& ray) const = 0;
};

}  // namespace LTRE

#endif