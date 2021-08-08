#ifndef _LTRE_FRESNEL_H
#define _LTRE_FRESNEL_H

#include "LTRE/math/vec3.hpp"

namespace LTRE {

class Fresnel {
 public:
  virtual Vec3 evaluate(float cosThetaI) const = 0;
  virtual float getIOR_I(float cosThetaI) const = 0;
  virtual float getIOR_T(float cosThetaI) const = 0;
};

class FresnelDielectric : public Fresnel {
 private:
  float iorI_;
  float iorT_;

 public:
  FresnelDielectric(float iorI, float iorT);

  Vec3 evaluate(float cosThetaI) const override;
  float getIOR_I(float cosThetaI) const override;
  float getIOR_T(float cosThetaI) const override;
};

// TODO: specify IOR with Vec3, then we can get colored reflectance
class FresnelConductor : public Fresnel {
 private:
  float iorI_;
  float iorT_;
  float extinctionT_;

 public:
  FresnelConductor(float iorI, float iorT, float extinctionT);

  Vec3 evaluate(float cosThetaI) const override;
  float getIOR_I(float cosThetaI) const override;
  float getIOR_T(float cosThetaI) const override;
};

class FresnelSchlick : public Fresnel {
 private:
  Vec3 f0_;

 public:
  FresnelSchlick();
  FresnelSchlick(const Vec3& f0);

  Vec3 evaluate(float cosThetaI) const override;
  float getIOR_I(float cosThetaI) const override;
  float getIOR_T(float cosThetaI) const override;
};

}  // namespace LTRE

#endif