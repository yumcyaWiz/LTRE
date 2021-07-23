#include "LTRE/sampling/rng.hpp"

namespace LTRE {

RNG::RNG() {
  state.state = 1;
  state.inc = 1;
}

RNG::RNG(uint64_t seed) {
  state.state = seed;
  state.inc = 1;
}

void RNG::setSeed(uint64_t seed) { state.state = seed; }

float RNG::getNext() {
  constexpr float divider = 1.0f / std::numeric_limits<uint32_t>::max();
  return pcg32_random_r(&state) * divider;
}

}  // namespace LTRE