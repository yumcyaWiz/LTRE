#include "LTRE/light/sky/uniform-sky.hpp"

namespace LTRE {

UniformSky::UniformSky(const Vec3& ke) : ke(ke) {}

Vec3 UniformSky::power() const { return PI_MUL_4 * 10000.0f * ke; }

Vec3 UniformSky::Le([[maybe_unused]] const Vec3& wi,
                    [[maybe_unused]] const SurfaceInfo& info) const {
  return ke;
}

Vec3 UniformSky::sampleDirection(const SurfaceInfo& surfInfo, Sampler& sampler,
                                 Vec3& dir, float& distToLight,
                                 float& pdf) const {
  // compute tangent space basis
  Vec3 t, b;
  orthonormalBasis(surfInfo.normal, t, b);

  // sample in tangent space
  Vec3 dir_l = sampleCosineHemisphere(sampler.getNext2D(), pdf);

  // local to world transform
  dir = localToWorld(dir_l, t, surfInfo.normal, b);

  // set very large value to distToLight
  distToLight = std::numeric_limits<float>::max();

  return ke;
}

}  // namespace LTRE