#ifndef _LTRE_FRESNEL_H
#define _LTRE_FRESNEL_H

#include "LTRE/math/vec3.hpp"

namespace LTRE {

class Fresnel {
 protected:
  float iorI_;
  float iorT_;

 public:
  Fresnel(float iorI, float iorT);

  float getIOR_I(float cosThetaI) const;
  float getIOR_T(float cosThetaI) const;

  virtual Vec3 evaluate(float cosThetaI) const = 0;
};

class FresnelDielectric : public Fresnel {
 public:
  FresnelDielectric(float iorI, float iorT);

  Vec3 evaluate(float cosThetaI) const override;
};

// TODO: specify IOR with Vec3, then we can get colored reflectance
class FresnelConductor : public Fresnel {
 private:
  float extinctionT_;

 public:
  FresnelConductor(float iorI, float iorT, float extinctionT);

  Vec3 evaluate(float cosThetaI) const override;
};

// TODO: handle iorI, iorT
class FresnelSchlick : public Fresnel {
 private:
  Vec3 f0_;

 public:
  FresnelSchlick();
  FresnelSchlick(const Vec3& f0);

  Vec3 evaluate(float cosThetaI) const override;
};

}  // namespace LTRE

#endif