#ifndef _LTRE_IBL_H
#define _LTRE_IBL_H
#include <filesystem>

//
#include "LTRE/core/texture.hpp"
#include "LTRE/light/light.hpp"
#include "LTRE/sampling/sampling.hpp"

namespace LTRE {

class IBL : public Light {
 private:
  const ImageTexture imageTexture;
  const float thetaOffset;
  const float phiOffset;
  const float scale;

 public:
  IBL(const std::filesystem::path& filepath, float thetaOffset = 0.0f,
      float phiOffset = 0.0f, float scale = 1.0f)
      : imageTexture{filepath},
        thetaOffset(thetaOffset),
        phiOffset(phiOffset),
        scale(scale) {}

  Vec3 power() const override {
    return PI_MUL_4 * 10000.0f * imageTexture.average();
  }

  Vec3 Le(const Vec3& wi,
          [[maybe_unused]] const SurfaceInfo& surfInfo) const override {
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

  Vec3 sampleDirection(const SurfaceInfo& surfInfo, Sampler& sampler, Vec3& dir,
                       float& distToLight, float& pdf) const override {
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
};

}  // namespace LTRE

#endif