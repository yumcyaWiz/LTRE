#ifndef _LTRE_AO_H
#define _LTRE_AO_H
#include "LTRE/integrator/integrator.hpp"

namespace LTRE {

class AO : public Integrator {
 private:
  float occulusionDistance;

 public:
  AO(float occulusionDistance);

  Vec3 integrate(const Ray& ray_in, const Scene& scene,
                 Sampler& sampler) const override;
};

}  // namespace LTRE

#endif