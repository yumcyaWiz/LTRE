#ifndef _LTRE_UNIFORM_H
#define _LTRE_UNIFORM_H
#include "LTRE/sampling/sampler.hpp"

namespace LTRE {

class UniformSampler : public Sampler {
 public:
  UniformSampler();
  UniformSampler(uint64_t seed);

  std::unique_ptr<Sampler> clone() const override;

  float getNext1D() override;
  Vec2 getNext2D() override;
};

}  // namespace LTRE

#endif