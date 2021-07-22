#include "LTRE/camera/pinhole-camera.hpp"

#include "spdlog/spdlog.h"

namespace LTRE {

PinholeCamera::PinholeCamera(const Vec3& camPos, const Vec3& camForward,
                             const Vec2& filmSize, float FOV)
    : Camera(camPos, camForward, filmSize) {
  // compute focal length from FOV
  f = (0.5f * filmSize[0]) / std::tan(0.5f * FOV);

  spdlog::info("[PinholeCamera] f: " + std::to_string(f));
}

Vec3 PinholeCamera::We([[maybe_unused]] const Vec2& uv,
                       [[maybe_unused]] const Vec3& wi) const {
  return Vec3(f * f);
}

bool PinholeCamera::sampleRay(const Vec2& uv, [[maybe_unused]] Sampler& sampler,
                              Ray& ray, Vec3& wi, float& pdf) const {
  const Vec3 sensorPos = this->sensorPos(uv);
  const Vec3 pinholePos = camPos + f * camForward;
  ray = Ray(sensorPos, normalize(pinholePos - sensorPos));
  wi = ray.direction;
  pdf = length2(pinholePos - sensorPos) / dot(ray.direction, camForward);
  return true;
}

}  // namespace LTRE