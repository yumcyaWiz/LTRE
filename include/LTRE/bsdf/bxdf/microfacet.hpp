#ifndef _LTRE_MICROFACET_H
#define _LTRE_MICROFACET_H
#include "LTRE/bsdf/bxdf/bxdf.hpp"
#include "LTRE/bsdf/bxdf/fresnel.hpp"

namespace LTRE {

// TODO: sample from visible normal
class MicrofacetDistribution {
 public:
  // half-vector distribution
  virtual float D(const Vec3& wh) const = 0;

  // lambda for smith masking function
  virtual float Lambda(const Vec3& w) const = 0;

  // sample direction with pdf propotional to D(wh)cos(wh)
  virtual Vec3 sample(const Vec2& uv, float& pdf) const = 0;
  float pdf(const Vec3& wh) const;

  // smith masking function
  float G1(const Vec3& w) const;

  // height-correlated masking-shadowing
  float G2(const Vec3& wo, const Vec3& wi) const;
};

class Beckmann : public MicrofacetDistribution {
 private:
  float alpha;

 public:
  Beckmann(float alpha);

  float D(const Vec3& wh) const override;
  float Lambda(const Vec3& w) const override;
  Vec3 sample(const Vec2& uv, float& pdf) const override;
};

class Berry : public MicrofacetDistribution {
 private:
  float alpha;

 public:
  Berry();
  Berry(float alpha);

  float D(const Vec3& wh) const override;
  float Lambda(const Vec3& w) const override;
  Vec3 sample(const Vec2& uv, float& pdf) const override;
};

class GGX : public MicrofacetDistribution {
 private:
  float alpha;

 public:
  GGX();
  GGX(float alpha);

  float D(const Vec3& wh) const override;
  float Lambda(const Vec3& w) const override;
  Vec3 sample(const Vec2& uv, float& pdf) const override;
};

class MicrofacetBRDF : public BxDF {
 private:
  const Fresnel* fresnel;
  const MicrofacetDistribution* distribution;

 public:
  MicrofacetBRDF();
  MicrofacetBRDF(const Fresnel* fresnel,
                 const MicrofacetDistribution* distribution);

  Vec3 f(const Vec3& wo, const Vec3& wi) const override;
  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
              float& pdf) const override;
  float pdf(const Vec3& wo, const Vec3& wi) const override;
  float reflectance(const Vec3& wo) const override;
};

// https://dl.acm.org/doi/10.5555/2383847.2383874
class MicrofacetBTDF : public BxDF {
 private:
  const Fresnel* fresnel;
  const MicrofacetDistribution* distribution;

 public:
  MicrofacetBTDF();
  MicrofacetBTDF(const Fresnel* fresnel,
                 const MicrofacetDistribution* distribution);

  Vec3 f(const Vec3& wo, const Vec3& wi) const override;
  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
              float& pdf) const override;
  float pdf(const Vec3& wo, const Vec3& wi) const override;
  float reflectance(const Vec3& wo) const override;
};

}  // namespace LTRE

#endif