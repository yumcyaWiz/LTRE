#ifndef _LTRE_MATERIAL_H
#define _LTRE_MATERIAL_H
#include <memory>

#include "LTRE/bsdf/bsdf.hpp"
#include "LTRE/bsdf/bxdf/diffuse.hpp"
#include "LTRE/bsdf/bxdf/disney.hpp"
#include "LTRE/core/texture.hpp"
#include "LTRE/math/vec3.hpp"
#include "LTRE/sampling/sampler.hpp"

namespace LTRE {

class Material {
 public:
  Material() {}

  virtual BSDF prepareBSDF(const IntersectInfo& info) const = 0;
  virtual Vec3 baseColor(const IntersectInfo& info) const = 0;

  // NOTE: having bsdf as private member does not work on multithread case
  Vec3 f(const Vec3& wo, const IntersectInfo& info, const Vec3& wi) const {
    const BSDF bsdf = prepareBSDF(info);
    return bsdf.f(wo, wi);
  }

  // NOTE: having bsdf as private member does not work on multithread case
  Vec3 sample(Sampler& sampler, const Vec3& wo, const IntersectInfo& info,
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

  BSDF prepareBSDF(const IntersectInfo& info) const override {
    BSDF bsdf;
    const Vec3 rho = _baseColor->sample(info);
    bsdf.add(std::make_shared<OrenNayer>(rho, roughness));
    return bsdf;
  }

  Vec3 baseColor(const IntersectInfo& info) const override {
    return _baseColor->sample(info);
  }
};

class DisneyPrincipledBRDF : public Material {
 private:
  const std::shared_ptr<Texture<Vec3>> _baseColor;
  const float roughness;
  /*
  const float subsurface;
  const float metallic;
  const float sheen;
  const float sheenTint;
  const float clearcoat;
  */

 public:
  DisneyPrincipledBRDF(const std::shared_ptr<Texture<Vec3>>& baseColor,
                       float roughness)
      : _baseColor(baseColor), roughness(roughness) {}

  BSDF prepareBSDF(const IntersectInfo& info) const override {
    BSDF bsdf;
    const Vec3 rho = _baseColor->sample(info);
    bsdf.add(std::make_shared<DisneyDiffuse>(rho, roughness));
    return bsdf;
  }

  Vec3 baseColor(const IntersectInfo& info) const override {
    return _baseColor->sample(info);
  }
};

}  // namespace LTRE

#endif