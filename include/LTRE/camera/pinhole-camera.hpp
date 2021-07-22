#ifndef _LTRE_PINHOLE_CAMERA_H
#define _LTRE_PINHOLE_CAMERA_H

#include "LTRE/camera/camera.hpp"

namespace LTRE {

class PinholeCamera : public Camera {
 private:
  float f;

 public:
  PinholeCamera(const Vec3& camPos, const Vec3& camForward,
                const Vec2& filmSize = Vec2(0.025, 0.025),
                float FOV = PI_MUL_2_INV);

  Vec3 We(const Vec2& uv, const Vec3& wi) const override;
  bool sampleRay(const Vec2& uv, Sampler& sampler, Ray& ray, Vec3& wi,
                 float& pdf) const override;
};

}  // namespace LTRE

#endif