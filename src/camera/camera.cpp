#include "LTRE/camera/camera.hpp"

#include "spdlog/spdlog.h"

namespace LTRE {

Vec3 Camera::sensorPos(const Vec2& uv) const {
  return camPos + uv[0] * 0.5f * filmSize[0] * camRight +
         uv[1] * 0.5f * filmSize[1] * camUp;
}

Camera::Camera(const Vec3& camPos, const Vec3& camForward, const Vec2& filmSize)
    : camPos(camPos), camForward(camForward), filmSize(filmSize) {
  camRight = normalize(cross(camForward, Vec3(0, 1, 0)));
  camUp = normalize(cross(camRight, camForward));

  spdlog::info("[Camera] camPos: " + camPos.toString());
  spdlog::info("[Camera] camForward: " + camForward.toString());
  spdlog::info("[Camera] camRight: " + camRight.toString());
  spdlog::info("[Camera] camUp: " + camUp.toString());
}

Vec3 Camera::getCameraPosition() const { return camPos; }

Vec3 Camera::getCameraForward() const { return camForward; }

}  // namespace LTRE