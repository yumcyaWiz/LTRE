#ifndef _LTRE_PT_H
#define _LTRE_PT_H
#include "LTRE/integrator/integrator.hpp"

namespace LTRE {

class PT : public Integrator {
 public:
  const int maxDepth;

  PT(int maxDepth = 100);

  Vec3 integrate(const Ray& ray_in, const Scene& scene,
                 Sampler& sampler) const override;
};

}  // namespace LTRE

#endif