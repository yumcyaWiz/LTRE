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
    if (scene.intersect(ray_in, info)) {
      const Primitive& prim = *info.hitPrimitive;

      // BRDF Sampling
      Vec3 wi;
      float pdf;
      const Vec3 bsdf =
          prim.sampleBSDF(-ray_in.direction, info, sampler, wi, pdf);

      // test occulusion
      Ray shadowRay = Ray(info.hitPos, wi);
      shadowRay.tmax = occulusionDistance;
      IntersectInfo shadowInfo;
      if (scene.intersect(shadowRay, shadowInfo)) {
        return Vec3(0);
      }

      const float cos = std::abs(dot(wi, info.hitNormal));
      return bsdf * cos / pdf;

    } else {
      return Vec3(1);
    }
  }
};

}  // namespace LTRE

#endif