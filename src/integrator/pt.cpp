#include "LTRE/integrator/pt.hpp"

namespace LTRE {

PT::PT(int maxDepth) : maxDepth(maxDepth) {}

Vec3 PT::integrate(const Ray& ray_in, const Scene& scene,
                   Sampler& sampler) const {
  Ray ray = ray_in;
  Vec3 throughput(1);
  Vec3 radiance(0);
  for (int i = 0; i < maxDepth; ++i) {
    // russian roulette
    const float russianRouletteProb = std::min(
        std::max(throughput[0], std::max(throughput[1], throughput[2])), 1.0f);
    if (sampler.getNext1D() > russianRouletteProb) break;
    throughput /= russianRouletteProb;

    IntersectInfo info;
    if (!scene.intersect(ray, info)) {
      // sky
      radiance += throughput * scene.getSkyRadiance(ray);
      break;
    }

    const Primitive& hitPrimitive = *info.hitPrimitive;

    // Le
    if (hitPrimitive.hasArealight()) {
      radiance += throughput * hitPrimitive.Le(ray.direction, info.surfaceInfo);
      break;
    }

    // BRDF Sampling
    Vec3 wi;
    float pdf;
    const Vec3 bsdf = hitPrimitive.sampleBSDF(-ray.direction, info.surfaceInfo,
                                              sampler, wi, pdf);

    // update throughput
    const float cos = std::max(dot(wi, info.surfaceInfo.normal), 0.0f);
    throughput *= bsdf * cos / pdf;

    // update ray
    ray = Ray(info.surfaceInfo.position, wi);
  }
  return radiance;
}

}  // namespace LTRE