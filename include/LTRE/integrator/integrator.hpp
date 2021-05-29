#ifndef _LTRE_INTEGRATOR_H
#define _LTRE_INTEGRATOR_H
#include <memory>

#include "LTRE/core/ray.hpp"
#include "LTRE/core/scene.hpp"
#include "LTRE/sampling/sampler.hpp"

namespace LTRE {

class Integrator {
 protected:
  std::shared_ptr<Sampler> sampler;

 public:
  virtual Vec3 integrate(const Ray& ray, const Scene& scene) const = 0;
};

};  // namespace LTRE

#endif