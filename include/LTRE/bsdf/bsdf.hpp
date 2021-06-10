#ifndef _LTRE_BSDF_H
#define _LTRE_BSDF_H
#include <cmath>
#include <memory>

#include "LTRE/bsdf/bxdf/bxdf.hpp"
#include "LTRE/core/intersect-info.hpp"
#include "LTRE/core/texture.hpp"

namespace LTRE {

class BSDF {
 private:
  std::shared_ptr<BxDF> bxdf;
  std::shared_ptr<Texture<Vec3>> _baseColor;
  float _roughness;

 public:
  BSDF(const std::shared_ptr<BxDF>& bxdf,
       const std::shared_ptr<Texture<Vec3>> baseColor, float roughness)
      : bxdf(bxdf), _baseColor(baseColor), _roughness(roughness) {}

  Vec3 baseColor(const IntersectInfo& info) const {
    return _baseColor->sample(info);
  }

  float roughness([[maybe_unused]] const IntersectInfo& info) const {
    return _roughness;
  }

  Vec3 bsdf(const Vec3& wo, const IntersectInfo& info, const Vec3& wi) const {
    BxDFArgs args;
    args.wo = wo;
    args.wi = wi;
    args.baseColor = baseColor(info);
    args.roughness = roughness(info);

    return bxdf->f(args);
  }

  Vec3 sample(const Vec3& wo, const IntersectInfo& info, Sampler& sampler,
              Vec3& wi, float& pdf) const {
    BxDFArgs args;
    args.wo = wo;
    args.baseColor = baseColor(info);
    args.roughness = roughness(info);

    const Vec3 bsdf = bxdf->sample(sampler, args, pdf);
    wi = args.wi;
    return bsdf;
  }
};

}  // namespace LTRE

#endif