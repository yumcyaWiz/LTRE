#include "LTRE/sampling/uniform.hpp"

namespace LTRE {

UniformSampler::UniformSampler() : Sampler() {}

UniformSampler::UniformSampler(uint64_t seed) : Sampler(seed) {}

std::unique_ptr<Sampler> UniformSampler::clone() const {
  return std::make_unique<UniformSampler>();
}

float UniformSampler::getNext1D() { return rng.getNext(); }

Vec2 UniformSampler::getNext2D() { return Vec2(rng.getNext(), rng.getNext()); }

}  // namespace LTRE