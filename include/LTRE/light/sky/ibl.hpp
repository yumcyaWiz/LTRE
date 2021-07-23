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
      float phiOffset = 0.0f, float scale = 1.0f);

  Vec3 power() const override;

  Vec3 Le(const Vec3& wi, const SurfaceInfo& surfInfo) const override;

  Vec3 sampleDirection(const SurfaceInfo& surfInfo, Sampler& sampler, Vec3& dir,
                       float& distToLight, float& pdf) const override;
};

}  // namespace LTRE

#endif