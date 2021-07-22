#ifndef _LTRE_LAMBERT_H
#define _LTRE_LAMBERT_H
#include "LTRE/bsdf/bxdf/bxdf.hpp"

namespace LTRE {

class Lambert : public BxDF {
 private:
  const Vec3 rho;

 public:
  Lambert(const Vec3& rho);

  Vec3 f(const Vec3& wo, const Vec3& wi) const override;
  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
              float& pdf) const override;
  float pdf(const Vec3& wo, const Vec3& wi) const override;
};

class OrenNayer : public BxDF {
 private:
  const Vec3 rho;
  const float sigma;
  float A;
  float B;

 public:
  OrenNayer(const Vec3& rho, float sigma);

  Vec3 f(const Vec3& wo, const Vec3& wi) const override;
  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
              float& pdf) const override;
  float pdf(const Vec3& wo, const Vec3& wi) const override;
};

}  // namespace LTRE

#endif