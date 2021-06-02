#ifndef _LTRE_UNIFORM_SKY_H
#define _LTRE_UNIFORM_SKY_H
#include "LTRE/sky/sky.hpp"

namespace LTRE {
class UniformSky : public Sky {
 private:
  const Vec3 color;

 public:
  UniformSky(const Vec3& color) : color(color) {}

  Vec3 radiance([[maybe_unused]] const Ray& ray) const override {
    return color;
  }
};
}  // namespace LTRE

#endif