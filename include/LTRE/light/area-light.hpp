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

  // return average power
  virtual Vec3 power() const override {
    return shape->surfaceArea() * le->average();
  }

  Vec3 Le(const Vec3& wi, const SurfaceInfo& info) const override {
    // return black when the surface is back faced
    return (dot(-wi, info.normal) > 0) * le->sample(info);
  }

  Vec3 sampleDirection(const SurfaceInfo& surfInfo, Sampler& sampler, Vec3& dir,
                       float& distToLight, float& pdf) const override {
    // sample point on shape
    float pdf_a;
    const SurfaceInfo sampledInfo = shape->samplePoint(sampler, pdf_a);
    dir = normalize(sampledInfo.position - surfInfo.position);
    distToLight = length(sampledInfo.position - surfInfo.position);

    // convert area pdf to solid angle pdf
    const float dist2 = distToLight * distToLight;
    const float cos = abs(dot(-dir, sampledInfo.normal));
    pdf = pdf_a * dist2 / cos;

    // return black when the sampled surface is back faced
    return (dot(-dir, sampledInfo.normal) > 0) * le->sample(sampledInfo);
  }
};

}  // namespace LTRE

#endif