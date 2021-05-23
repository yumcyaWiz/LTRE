#ifndef _LTRE_PINHOLE_CAMERA_H
#define _LTRE_PINHOLE_CAMERA_H
#include "LTRE/camera/camera.hpp"

namespace LTRE {

class PinholeCamera : public Camera {
 private:
  const float f;

 public:
  PinholeCamera(const Vec3& camPos, const Vec3& camForward)
      : Camera(camPos, camForward), f(1.0f) {}

  bool sampleRay(const Vec2& uv, Ray& ray, float& pdf) const override {
    const Vec3 sensorPos = camPos + uv[0] * camRight + uv[1] * camUp;
    const Vec3 pinholePos = camPos + f * camForward;
    ray = Ray(sensorPos, normalize(pinholePos - sensorPos));
    pdf = 1.0f;
    return true;
  }
};

}  // namespace LTRE

#endif