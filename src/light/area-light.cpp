#include "LTRE/light/area-light.hpp"

namespace LTRE {

AreaLight::AreaLight(const std::shared_ptr<Texture<Vec3>>& le,
                     const std::shared_ptr<Shape>& shape)
    : le(le), shape(shape) {}

Vec3 AreaLight::power() const { return shape->surfaceArea() * le->average(); }

Vec3 AreaLight::Le(const Vec3& wi, const SurfaceInfo& info) const {
  // return black when the surface is back faced
  return (dot(-wi, info.normal) > 0) * le->sample(info);
}

Vec3 AreaLight::sampleDirection(const SurfaceInfo& surfInfo, Sampler& sampler,
                                Vec3& dir, float& distToLight,
                                float& pdf) const {
  // sample point on shape
  float pdf_area;
  const SurfaceInfo sampledInfo = shape->samplePoint(sampler, pdf_area);
  dir = normalize(sampledInfo.position - surfInfo.position);
  distToLight = std::max(
      length(sampledInfo.position - surfInfo.position) - RAY_EPS, 0.0f);

  // convert area pdf to solid angle pdf
  const float dist2 = distToLight * distToLight;
  const float cos = std::abs(dot(-dir, sampledInfo.normal));
  pdf = pdf_area * dist2 / cos;

  // return black when the sampled surface is back faced
  return (dot(-dir, sampledInfo.normal) > 0) * le->sample(sampledInfo);
}

}  // namespace LTRE