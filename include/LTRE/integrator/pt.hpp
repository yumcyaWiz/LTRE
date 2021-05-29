#ifndef _LTRE_PT_H
#define _LTRE_PT_H
#include "LTRE/integrator/integrator.hpp"

namespace LTRE {

class PT : public Integrator {
 public:
  const int maxDepth;

  PT(int maxDepth = 100) : maxDepth(maxDepth) {}

  Vec3 integrate(const Ray& ray_in, const Scene& scene) const override {
    Vec3 throughput(1);
    Ray ray = ray_in;
    Vec3 radiance(0);
    for (int i = 0; i < maxDepth; ++i) {
      // russian roulette
      const float russianRouletteProb = std::min(
          std::max(throughput[0], std::max(throughput[1], throughput[2])),
          1.0f);
      if (sampler->getNext1D() < russianRouletteProb) break;
      throughput /= russianRouletteProb;

      IntersectInfo info;
      if (scene.intersect(ray, info)) {
        const Primitive& prim = *info.hitPrimitive;

        // BRDF Sampling
        Vec3 wi;
        float pdf;
        const Vec3 bsdf =
            prim.sampleBSDF(-ray.direction, info, *sampler, wi, pdf);

        const float cos = std::abs(dot(wi, info.hitNormal));

        // update throughput
        throughput *= bsdf * cos / pdf;
      }
      // sky
      else {
        radiance = throughput * Vec3(1);
        break;
      }
    }
    return radiance;
  }
};

}  // namespace LTRE

#endif