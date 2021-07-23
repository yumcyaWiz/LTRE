#include "LTRE/light/sky/ibl.hpp"

namespace LTRE {

IBL::IBL(const std::filesystem::path& filepath, float thetaOffset,
         float phiOffset, float scale)
    : imageTexture{filepath},
      thetaOffset(thetaOffset),
      phiOffset(phiOffset),
      scale(scale) {}

Vec3 IBL::power() const { return PI_MUL_4 * 10000.0f * imageTexture.average(); }

Vec3 IBL::Le(const Vec3& wi,
             [[maybe_unused]] const SurfaceInfo& surfInfo) const {
  // compute spherical coordinate
  float theta, phi;
  cartesianToSpherical(wi, theta, phi);
  theta = std::fmod(theta + thetaOffset, PI);
  phi = std::fmod(phi + phiOffset, PI_MUL_2);

  // compute texture coordinate
  // TODO: more nice way?
  SurfaceInfo temp;
  const float u = phi * PI_MUL_2_INV;
  const float v = theta * PI_INV;
  temp.uv = Vec2(u, v);

  return scale * imageTexture.sample(temp);
}

Vec3 IBL::sampleDirection(const SurfaceInfo& surfInfo, Sampler& sampler,
                          Vec3& dir, float& distToLight, float& pdf) const {
  // compute tangent space basis
  Vec3 t, b;
  orthonormalBasis(surfInfo.normal, t, b);

  // sample in tangent space
  Vec3 dir_l = sampleCosineHemisphere(sampler.getNext2D(), pdf);

  // local to world transform
  dir = localToWorld(dir_l, t, surfInfo.normal, b);

  // set very large value to distToLight
  distToLight = std::numeric_limits<float>::max();

  // TODO: more nice way?
  SurfaceInfo _dummy;
  return Le(dir, _dummy);
}

}  // namespace LTRE