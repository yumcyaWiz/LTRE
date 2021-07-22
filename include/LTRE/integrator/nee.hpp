#ifndef _LTRE_NEE_H
#define _LTRE_NEE_H
#include "LTRE/integrator/integrator.hpp"

namespace LTRE {

class NEE : public Integrator {
 public:
  const int maxDepth;

  NEE(int maxDepth = 100);

  Vec3 integrate(const Ray& ray_in, const Scene& scene,
                 Sampler& sampler) const override;
};

}  // namespace LTRE

#endif