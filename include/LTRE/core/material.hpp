#ifndef _LTRE_MATERIAL_H
#define _LTRE_MATERIAL_H
#include <memory>

#include "LTRE/bsdf/bsdf.hpp"
#include "LTRE/core/texture.hpp"
#include "LTRE/math/vec3.hpp"
#include "LTRE/sampling/sampler.hpp"

namespace LTRE {

class Material {
 public:
  Material() {}

  virtual BSDF prepareBSDF(const SurfaceInfo& info) const = 0;
  virtual Vec3 baseColor(const SurfaceInfo& info) const = 0;

  Vec3 f(const Vec3& wo, const Vec3& wi, const SurfaceInfo& info) const;
  Vec3 sample(Sampler& sampler, const Vec3& wo, const SurfaceInfo& info,
              Vec3& wi, float& pdf) const;
};

class Diffuse : public Material {
 private:
  const std::shared_ptr<Texture<Vec3>> _baseColor;
  const float roughness;

 public:
  Diffuse(const std::shared_ptr<Texture<Vec3>>& baseColor, float roughness);

  BSDF prepareBSDF(const SurfaceInfo& info) const override;
  Vec3 baseColor(const SurfaceInfo& info) const override;
};

class Metal : public Material {
 private:
  const std::shared_ptr<Texture<Vec3>> baseColor_;
  const float roughness_;

 public:
  Metal(const std::shared_ptr<Texture<Vec3>>& baseColor, float roughness);

  BSDF prepareBSDF(const SurfaceInfo& info) const override;
  Vec3 baseColor(const SurfaceInfo& info) const override;
};

// TODO: add baseColor
class Glass : public Material {
 private:
  const float ior_;
  const float roughness_;

 public:
  Glass(float ior, float roughness);

  BSDF prepareBSDF(const SurfaceInfo& info) const override;
  Vec3 baseColor(const SurfaceInfo& info) const override;
};

class DisneyPrincipledBRDF : public Material {
 private:
  const std::shared_ptr<Texture<Vec3>> baseColor_;
  const float roughness_;
  const float subsurface_;
  const float metallic_;
  const float sheen_;
  const float sheenTint_;
  const float specular_;
  const float specularTint_;
  const float clearcoat_;
  const float clearcoatGloss_;

 public:
  DisneyPrincipledBRDF(const std::shared_ptr<Texture<Vec3>>& baseColor,
                       float roughness, float subsurface, float metallic,
                       float sheen, float sheenTint, float specular,
                       float specularTint, float clearcoat,
                       float clearcoatGloss);

  BSDF prepareBSDF(const SurfaceInfo& info) const override;
  Vec3 baseColor(const SurfaceInfo& info) const override;
};

}  // namespace LTRE

#endif