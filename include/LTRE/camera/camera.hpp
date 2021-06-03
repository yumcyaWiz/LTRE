#ifndef _LTRE_CAMERA_H
#define _LTRE_CAMERA_H
#include "LTRE/core/ray.hpp"
#include "LTRE/math/vec2.hpp"
#include "LTRE/math/vec3.hpp"
#include "LTRE/sampling/sampler.hpp"

namespace LTRE {

class Camera {
 protected:
  Vec3 camPos;
  Vec3 camForward;
  Vec3 camRight;
  Vec3 camUp;

  const Vec2 filmSize;

  Vec3 sensorPos(const Vec2& uv) const {
    return camPos + uv[0] * 0.5f * filmSize[0] * camRight +
           uv[1] * 0.5f * filmSize[1] * camUp;
  }

 public:
  Camera(const Vec3& camPos, const Vec3& camForward,
         const Vec2& filmSize = Vec2(0.025, 0.025))
      : camPos(camPos), camForward(camForward), filmSize(filmSize) {
    camRight = normalize(cross(camForward, Vec3(0, 1, 0)));
    camUp = normalize(cross(camRight, camForward));
  }

  Vec3 getCameraPosition() const { return camPos; }
  Vec3 getCameraForward() const { return camForward; }

  virtual Vec3 We(const Vec2& uv, const Vec3& wi) const = 0;

  virtual bool sampleRay(const Vec2& uv, Sampler& sampler, Ray& ray,
                         float& pdf) const = 0;

  virtual void focus([[maybe_unused]] const Vec3& p) {}
};

}  // namespace LTRE

#endif