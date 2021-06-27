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

  Vec3 radiance(const SurfaceInfo& info) const override {
    return le->sample(info);
  }

  Vec3 sampleDirection(const Vec3& pos, Sampler& sampler, Vec3& dir,
                       float& distToLight, float& pdf) const override {
    // sample point on shape
    float pdf_a;
    const SurfaceInfo info = shape->samplePoint(sampler, pdf_a);
    dir = normalize(info.position - pos);
    distToLight = length(info.position - pos);

    // convert area pdf to solid angle pdf
    const float dist2 = distToLight * distToLight;
    const float cos = abs(dot(-dir, info.normal));
    pdf = pdf_a * dist2 / cos;

    return le->sample(info);
  }
};

}  // namespace LTRE

#endif