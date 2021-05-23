#ifndef _LTRE_LAMBERT_H
#define _LTRE_LAMBERT_H
#include "LTRE/bsdf/bsdf.hpp"

namespace LTRE {

class Lambert : public BSDF {
 public:
  const Vec3 rho;

  Lambert(const Vec3& rho) : rho(rho) {}
};

}  // namespace LTRE

#endif