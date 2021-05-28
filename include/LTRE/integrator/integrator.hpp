#ifndef _LTRE_INTEGRATOR_H
#define _LTRE_INTEGRATOR_H
#include "LTRE/core/ray.hpp"

namespace LTRE {

class Integrator {
 public:
  virtual float integrate(const Ray& ray) const = 0;
};

};  // namespace LTRE

#endif