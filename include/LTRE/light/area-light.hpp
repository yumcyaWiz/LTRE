#ifndef _LTRE_AREA_LIGHT_H
#define _LTRE_AREA_LIGHT_H
#include "LTRE/core/texture.hpp"
#include "LTRE/light/light.hpp"
#include "LTRE/shape/shape.hpp"

namespace LTRE {

class AreaLight : public Light {
 private:
  const std::shared_ptr<Texture<Vec3>> le;
  const std::shared_ptr<Shape> shape;

 public:
  AreaLight(const std::shared_ptr<Texture<Vec3>>& le,
            const std::shared_ptr<Shape>& shape)
      : le(le), shape(shape) {}

  Vec3 radiance(const IntersectInfo& info) const override {
    return le->sample(info);
  }
};

}  // namespace LTRE

#endif