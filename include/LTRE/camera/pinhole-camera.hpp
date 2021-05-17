#ifndef _LTRE_PINHOLE_CAMERA_H
#define _LTRE_PINHOLE_CAMERA_H
#include "LTRE/camera/camera.hpp"

namespace LTRE {

class PinholeCamera : public Camera {
 private:
  float f;

 public:
  PinholeCamera(const Vec3& camPos, const Vec3& camForward)
      : Camera(camPos, camForward), f(1.0f) {}

  bool sampleRay(float u, float v, Ray& ray, float& pdf) const override {
    const Vec3 sensorPos = camPos + u * camRight + v * camUp;
    const Vec3 pinholePos = camPos + f * camForward;
    ray = Ray(sensorPos, normalize(pinholePos - sensorPos));
    pdf = 1.0f;
    return true;
  }
};

}  // namespace LTRE

#endif