#include "LTRE/core/material.hpp"

#include "LTRE/bsdf/bxdf/diffuse.hpp"
#include "LTRE/bsdf/bxdf/disney.hpp"
#include "LTRE/bsdf/bxdf/microfacet.hpp"

namespace LTRE {

// NOTE: having bsdf as private member does not work on multithread case
Vec3 Material::f(const Vec3& wo, const Vec3& wi,
                 const SurfaceInfo& info) const {
  const BSDF bsdf = prepareBSDF(info);
  return bsdf.f(wo, wi);
}

// NOTE: having bsdf as private member does not work on multithread case
Vec3 Material::sample(Sampler& sampler, const Vec3& wo, const SurfaceInfo& info,
                      Vec3& wi, float& pdf) const {
  const BSDF bsdf = prepareBSDF(info);
  return bsdf.sample(sampler, wo, wi, pdf);
}

Diffuse::Diffuse(const std::shared_ptr<Texture<Vec3>>& baseColor,
                 float roughness)
    : _baseColor(baseColor), roughness(roughness) {}

BSDF Diffuse::prepareBSDF(const SurfaceInfo& info) const {
  BSDF bsdf;
  const Vec3 rho = _baseColor->sample(info);
  bsdf.add(std::make_shared<OrenNayer>(rho, roughness), 1.0f);
  return bsdf;
}

Vec3 Diffuse::baseColor(const SurfaceInfo& info) const {
  return _baseColor->sample(info);
}

Metal::Metal(const std::shared_ptr<Texture<Vec3>>& baseColor, float roughness)
    : baseColor_(baseColor), roughness_(roughness) {}

BSDF Metal::prepareBSDF(const SurfaceInfo& info) const {
  BSDF bsdf;
  const Vec3 rho = baseColor_->sample(info);
  const auto F = std::make_shared<FresnelSchlick>(rho);
  const auto D =
      std::make_shared<GGX>(std::max(roughness_ * roughness_, 0.001f));
  const auto bxdf = std::make_shared<MicrofacetBRDF>(F.get(), D.get());
  bsdf.add(bxdf, 1.0f);
  return bsdf;
}

Vec3 Metal::baseColor(const SurfaceInfo& info) const {
  return baseColor_->sample(info);
}

Glass::Glass(float ior, float roughness) : ior_(ior), roughness_(roughness) {}

BSDF Glass::prepareBSDF([[maybe_unused]] const SurfaceInfo& info) const {
  BSDF bsdf;
  const auto F = std::make_shared<FresnelDielectric>(1.0f, ior_);
  const auto D =
      std::make_shared<GGX>(std::max(roughness_ * roughness_, 0.001f));
  const auto brdf = std::make_shared<MicrofacetBRDF>(F.get(), D.get());
  const auto btdf = std::make_shared<MicrofacetBTDF>(F.get(), D.get());
  bsdf.add(brdf, 1.0f);
  bsdf.add(btdf, 1.0f);
  return bsdf;
}

Vec3 Glass::baseColor([[maybe_unused]] const SurfaceInfo& info) const {
  return Vec3(1);
}

DisneyPrincipledBRDF::DisneyPrincipledBRDF(
    const std::shared_ptr<Texture<Vec3>>& baseColor, float roughness,
    float subsurface, float metallic, float sheen, float sheenTint,
    float specular, float specularTint, float clearcoat, float clearcoatGloss)
    : baseColor_(baseColor),
      roughness_(roughness),
      subsurface_(subsurface),
      metallic_(metallic),
      sheen_(sheen),
      sheenTint_(sheenTint),
      specular_(specular),
      specularTint_(specularTint),
      clearcoat_(clearcoat),
      clearcoatGloss_(clearcoatGloss) {}

BSDF DisneyPrincipledBRDF::prepareBSDF(const SurfaceInfo& info) const {
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
  bsdf.add(std::make_shared<DisneyClearcoat>(clearcoat_, clearcoatGloss_),
           kClearcoat);
  return bsdf;
}

Vec3 DisneyPrincipledBRDF::baseColor(const SurfaceInfo& info) const {
  return baseColor_->sample(info);
}

}  // namespace LTRE