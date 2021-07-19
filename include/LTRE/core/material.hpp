#ifndef _LTRE_MATERIAL_H
#define _LTRE_MATERIAL_H
#include <memory>

#include "LTRE/bsdf/bsdf.hpp"
#include "LTRE/bsdf/bxdf/diffuse.hpp"
#include "LTRE/bsdf/bxdf/disney.hpp"
#include "LTRE/bsdf/bxdf/microfacet.hpp"
#include "LTRE/core/texture.hpp"
#include "LTRE/math/vec3.hpp"
#include "LTRE/sampling/sampler.hpp"

namespace LTRE {

class Material {
 public:
  Material() {}

  virtual BSDF prepareBSDF(const SurfaceInfo& info) const = 0;
  virtual Vec3 baseColor(const SurfaceInfo& info) const = 0;

  // NOTE: having bsdf as private member does not work on multithread case
  Vec3 f(const Vec3& wo, const Vec3& wi, const SurfaceInfo& info) const {
    const BSDF bsdf = prepareBSDF(info);
    return bsdf.f(wo, wi);
  }

  // NOTE: having bsdf as private member does not work on multithread case
  Vec3 sample(Sampler& sampler, const Vec3& wo, const SurfaceInfo& info,
              Vec3& wi, float& pdf) const {
    const BSDF bsdf = prepareBSDF(info);
    return bsdf.sample(sampler, wo, wi, pdf);
  }
};

class Diffuse : public Material {
 private:
  const std::shared_ptr<Texture<Vec3>> _baseColor;
  const float roughness;

 public:
  Diffuse(const std::shared_ptr<Texture<Vec3>>& baseColor, float roughness)
      : _baseColor(baseColor), roughness(roughness) {}

  BSDF prepareBSDF(const SurfaceInfo& info) const override {
    BSDF bsdf;
    const Vec3 rho = _baseColor->sample(info);
    bsdf.add(std::make_shared<OrenNayer>(rho, roughness), 1.0f);
    return bsdf;
  }

  Vec3 baseColor(const SurfaceInfo& info) const override {
    return _baseColor->sample(info);
  }
};

class Metal : public Material {
 private:
  const std::shared_ptr<Texture<Vec3>> baseColor_;
  const float roughness_;

 public:
  Metal(const std::shared_ptr<Texture<Vec3>>& baseColor, float roughness)
      : baseColor_(baseColor), roughness_(roughness) {}

  BSDF prepareBSDF(const SurfaceInfo& info) const override {
    BSDF bsdf;
    const Vec3 rho = baseColor_->sample(info);
    const auto F = std::make_shared<FresnelSchlick>(rho);
    const auto D =
        std::make_shared<GGX>(std::max(roughness_ * roughness_, 0.001f));
    const auto bxdf = std::make_shared<MicrofacetBRDF>(F.get(), D.get());
    bsdf.add(bxdf, 1.0f);
    return bsdf;
  }

  Vec3 baseColor(const SurfaceInfo& info) const override {
    return baseColor_->sample(info);
  }
};

class DisneyPrincipledBRDF : public Material {
 private:
  const std::shared_ptr<Texture<Vec3>> baseColor_;
  const float roughness_;
  const float subsurface_;
  const float metallic_;
  const float sheen_;
  const float sheenTint_;
  float specular_;
  float specularTint_;
  const float clearcoat_;

 public:
  DisneyPrincipledBRDF(const std::shared_ptr<Texture<Vec3>>& baseColor,
                       float roughness, float subsurface, float metallic,
                       float sheen, float sheenTint, float specular,
                       float specularTint, float clearcoat)
      : baseColor_(baseColor),
        roughness_(roughness),
        subsurface_(subsurface),
        metallic_(metallic),
        sheen_(sheen),
        sheenTint_(sheenTint),
        specular_(specular),
        specularTint_(specularTint),
        clearcoat_(clearcoat) {}

  BSDF prepareBSDF(const SurfaceInfo& info) const override {
    BSDF bsdf;
    const Vec3 baseColor = baseColor_->sample(info);
    const float kDiffuse = (1.0f - metallic_) * (1.0f - subsurface_);
    const float kSubsurface = (1.0f - metallic_) * subsurface_;
    const float kSheen = (1.0f - metallic_);
    const float kSpecular = 1.0f;
    const float kClearcoat = 1.0f;

    bsdf.add(std::make_shared<DisneyDiffuse>(baseColor, roughness_), kDiffuse);
    bsdf.add(std::make_shared<DisneySubsurface>(baseColor, subsurface_),
             kSubsurface);
    bsdf.add(std::make_shared<DisneySheen>(baseColor, sheen_, sheenTint_),
             kSheen);
    bsdf.add(std::make_shared<DisneySpecular>(baseColor, roughness_, specular_,
                                              specularTint_, metallic_, 0),
             kSpecular);
    bsdf.add(std::make_shared<DisneyClearcoat>(clearcoat_), kClearcoat);
    return bsdf;
  }

  Vec3 baseColor(const SurfaceInfo& info) const override {
    return baseColor_->sample(info);
  }
};

}  // namespace LTRE

#endif