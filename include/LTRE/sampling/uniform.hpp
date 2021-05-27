#ifndef _LTRE_UNIFORM_H
#include "LTRE/sampling/sampler.hpp"

namespace LTRE {

class UniformSampler : public Sampler {
 public:
  UniformSampler() : Sampler() {}
  UniformSampler(uint64_t seed) : Sampler(seed) {}

  float getNext1D() override { return rng.getNext(); }
  Vec2 getNext2D() override { return Vec2(rng.getNext(), rng.getNext()); }
};

}  // namespace LTRE

#endif