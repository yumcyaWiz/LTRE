#ifndef _LTRE_AO_H
#define _LTRE_AO_H
#include "LTRE/integrator/integrator.hpp"

namespace LTRE {

class AO : public Integrator {
 private:
  float occulusionDistance;

 public:
  AO(float occulusionDistance) : occulusionDistance(occulusionDistance) {}

  Vec3 integrate(const Ray& ray_in, const Scene& scene,
                 Sampler& sampler) const override {
    IntersectInfo info;
    if (!scene.intersect(ray_in, info)) {
      // sky
      return scene.getSkyRadiance(ray_in);
    }

    const Primitive& hitPrimitive = *info.hitPrimitive;

    // BRDF Sampling
    Vec3 wi;
    float pdf;
    const Vec3 bsdf = hitPrimitive.sampleBSDF(
        -ray_in.direction, info.surfaceInfo, sampler, wi, pdf);

    // test occulusion
    const Ray shadowRay = Ray(info.surfaceInfo.position, wi);
    shadowRay.tmax = occulusionDistance;
    if (scene.intersectP(shadowRay)) {
      return Vec3(0);
    }

    const float cos = std::max(dot(wi, info.surfaceInfo.normal), 0.0f);
    return bsdf * cos / pdf;
  }
};

}  // namespace LTRE

#endif