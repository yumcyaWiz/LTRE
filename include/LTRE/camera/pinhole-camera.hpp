#ifndef _LTRE_PINHOLE_CAMERA_H
#define _LTRE_PINHOLE_CAMERA_H
#include <cmath>

#include "LTRE/camera/camera.hpp"

namespace LTRE {

class PinholeCamera : public Camera {
 private:
  float f;

 public:
  PinholeCamera(const Vec3& camPos, const Vec3& camForward,
                const Vec2& filmSize = Vec2(0.025, 0.025),
                float FOV = PI_MUL_2_INV)
      : Camera(camPos, camForward, filmSize) {
    // compute focal length from FOV
    f = (0.5f * filmSize[0]) / std::tan(0.5f * FOV);

    spdlog::info("[PinholeCamera] f: " + std::to_string(f));
  }

  Vec3 We([[maybe_unused]] const Vec2& uv,
          [[maybe_unused]] const Vec3& wi) const override {
    return Vec3(f * f);
  }

  bool sampleRay(const Vec2& uv, [[maybe_unused]] Sampler& sampler, Ray& ray,
                 Vec3& wi, float& pdf) const override {
    const Vec3 sensorPos = this->sensorPos(uv);
    const Vec3 pinholePos = camPos + f * camForward;
    ray = Ray(sensorPos, normalize(pinholePos - sensorPos));
    wi = ray.direction;
    pdf = length2(pinholePos - sensorPos) / dot(ray.direction, camForward);
    return true;
  }
};

}  // namespace LTRE

#endif