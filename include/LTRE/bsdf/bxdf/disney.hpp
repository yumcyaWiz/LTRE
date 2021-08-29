#ifndef _LTRE_DISNEY_H
#define _LTRE_DISNEY_H
#include "LTRE/bsdf/bxdf/bxdf.hpp"
#include "LTRE/bsdf/bxdf/fresnel.hpp"

namespace LTRE {

class DisneyDiffuse : public BxDF {
 private:
  const Vec3 baseColor;
  const float roughness;

 public:
  DisneyDiffuse(const Vec3& baseColor, float roughness);

  Vec3 f(const Vec3& wo, const Vec3& wi) const override;
  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
              float& pdf) const override;
  float pdf(const Vec3& wo, const Vec3& wi) const override;
  float reflectance(const Vec3& wo) const override;
};

class DisneySubsurface : public BxDF {
 private:
  const Vec3 baseColor;
  const float roughness;

 public:
  DisneySubsurface(const Vec3& baseColor, float roughness);

  Vec3 f(const Vec3& wo, const Vec3& wi) const override;
  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
              float& pdf) const override;
  float pdf([[maybe_unused]] const Vec3& wo, const Vec3& wi) const override;
  float reflectance(const Vec3& wo) const override;
};

class DisneySheen : public BxDF {
 private:
  const Vec3 baseColor;
  const float sheen;
  const float sheenTint;
  Vec3 rho_tint;
  Vec3 rho_sheen;

 public:
  DisneySheen(const Vec3& baseColor, float sheen, float sheenTint);

  Vec3 f(const Vec3& wo, const Vec3& wi) const override;
  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
              float& pdf) const override;
  float pdf([[maybe_unused]] const Vec3& wo, const Vec3& wi) const override;
  float reflectance(const Vec3& wo) const override;
};

class DisneySpecular : public BxDF {
 private:
  float alpha;
  FresnelSchlick F;

  // GGX D
  float D(const Vec3& wh) const;
  Vec3 sample(const Vec2& uv, float& pdf) const;

  // GGX Lambda
  float Lambda(const Vec3& w) const;
  float G1(const Vec3& w) const;
  // separable masking-shadowing
  float G2(const Vec3& wo, const Vec3& wi) const;

 public:
  DisneySpecular(const Vec3& baseColor, float roughness, float specular,
                 float specularTint, float metallic, float anisotropic);

  Vec3 f(const Vec3& wo, const Vec3& wi) const override;
  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
              float& pdf) const override;
  float pdf(const Vec3& wo, const Vec3& wi) const override;
  float reflectance(const Vec3& wo) const override;
};

class DisneyClearcoat : public BxDF {
 private:
  float clearcoat;
  float alpha_D;
  float alpha_G;
  FresnelSchlick F;

  // berry
  float D(const Vec3& wh) const;

  // GGX Lambda
  float Lambda(const Vec3& w) const;
  float G1(const Vec3& w) const;
  // separable masking-shadowing
  float G2(const Vec3& wo, const Vec3& wi) const;

 public:
  DisneyClearcoat(float clearcoat, float clearcoatGloss);

  Vec3 f(const Vec3& wo, const Vec3& wi) const override;
  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
              float& pdf) const override;
  float pdf(const Vec3& wo, const Vec3& wi) const override;
  float reflectance(const Vec3& wo) const override;
};

}  // namespace LTRE

#endif