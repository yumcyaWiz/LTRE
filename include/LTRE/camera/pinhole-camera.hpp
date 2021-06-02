#ifndef _LTRE_PINHOLE_CAMERA_H
#define _LTRE_PINHOLE_CAMERA_H
#include <cmath>

#include "LTRE/camera/camera.hpp"

namespace LTRE {

class PinholeCamera : public Camera {
 private:
  float f;

  float computeDistanceFromFOV(float FOV) const {
    return (0.5f * filmSize[0]) / std::tan(0.5f * FOV);
  }

 public:
  PinholeCamera(const Vec3& camPos, const Vec3& camForward,
                const Vec2& filmSize = Vec2(0.025, 0.025),
                float FOV = PI_MUL_2_INV)
      : Camera(camPos, camForward, filmSize) {
    f = computeDistanceFromFOV(FOV);
  }

  bool sampleRay(const Vec2& uv, [[maybe_unused]] Sampler& sampler, Ray& ray,
                 float& pdf) const override {
    const Vec3 sensorPos = this->sensorPos(uv);
    const Vec3 pinholePos = camPos + f * camForward;
    ray = Ray(sensorPos, normalize(pinholePos - sensorPos));
    pdf = 1.0f;
    return true;
  }
};

}  // namespace LTRE

#endif