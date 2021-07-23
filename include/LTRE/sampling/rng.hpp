#ifndef _LTRE_RNG_H
#define _LTRE_RNG_H
#include <cstdint>
#include <limits>

namespace LTRE {

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
typedef struct {
  uint64_t state;
  uint64_t inc;
} pcg32_random_t;

inline uint32_t pcg32_random_r(pcg32_random_t* rng) {
  uint64_t oldstate = rng->state;
  // Advance internal state
  rng->state = oldstate * 6364136223846793005ULL + (rng->inc | 1);
  // Calculate output function (XSH RR), uses old state for max ILP
  uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
  uint32_t rot = oldstate >> 59u;
  return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

class RNG {
 private:
  pcg32_random_t state;

 public:
  RNG();
  RNG(uint64_t seed);

  void setSeed(uint64_t seed);
  float getNext();
};

}  // namespace LTRE

#endif