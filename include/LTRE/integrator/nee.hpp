#ifndef _LTRE_NEE_H
#define _LTRE_NEE_H
#include "LTRE/integrator/integrator.hpp"

namespace LTRE {

class NEE : public Integrator {
 public:
  const int maxDepth;

  NEE(int maxDepth = 100) : maxDepth(maxDepth) {}

  Vec3 integrate(const Ray& ray_in, const Scene& scene,
                 Sampler& sampler) const override {
    Ray ray = ray_in;
    Vec3 throughput(1);
    Vec3 radiance(0);
    for (int depth = 0; depth < maxDepth; ++depth) {
      // russian roulette
      const float russianRouletteProb = std::min(
          std::max(throughput[0], std::max(throughput[1], throughput[2])),
          1.0f);
      if (sampler.getNext1D() > russianRouletteProb) break;
      throughput /= russianRouletteProb;

      IntersectInfo info;
      if (!scene.intersect(ray, info)) {
        // sky
        radiance += throughput * scene.getSkyRadiance(ray);
        break;
      }

      const Primitive& prim = *info.hitPrimitive;

      // first hit light case
      if (depth == 0 && prim.hasArealight()) {
        radiance += throughput * prim.Le(info);
        break;
      }

      // light sampling
      {
        // sample light
        float lightChoosePdf;
        const std::shared_ptr<Light> light =
            scene.sampleLight(sampler, lightChoosePdf);

        // sample direction by light
        float lightPdf;
        const Vec3 dir = light->sampleDirection(info, sampler, lightPdf);

        // test visibility
        Ray shadowRay(info.hitPos, dir);
        IntersectInfo shadowInfo;
      }

      // BRDF Sampling
      Vec3 wi;
      float pdf;
      const Vec3 bsdf = prim.sampleBSDF(-ray.direction, info, sampler, wi, pdf);

      // update throughput
      const float cos = std::abs(dot(wi, info.hitNormal));
      throughput *= bsdf * cos / pdf;

      // update ray
      ray = Ray(info.hitPos, wi);
    }
    return radiance;
  }
};

}  // namespace LTRE

#endif