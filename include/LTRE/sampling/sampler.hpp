#ifndef _LTRE_SAMPLER_H
#define _LTRE_SAMPLER_H
#include <memory>

#include "LTRE/math/vec2.hpp"
#include "LTRE/sampling/rng.hpp"

namespace LTRE {

class Sampler {
 protected:
  RNG rng;

 public:
  Sampler() {}
  Sampler(uint64_t seed) : rng(seed) {}

  void setSeed(uint64_t seed) { rng.setSeed(seed); }

  virtual std::unique_ptr<Sampler> clone() const = 0;

  virtual float getNext1D() = 0;
  virtual Vec2 getNext2D() = 0;
};

};  // namespace LTRE

#endif