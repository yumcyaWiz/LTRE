#include "LTRE/camera/thin-lens.hpp"

#include "LTRE/sampling/sampling.hpp"
#include "spdlog/spdlog.h"

namespace LTRE {

ThinLensCamera::ThinLensCamera(const Vec3& camPos, const Vec3& camForward,
                               const Vec2& filmSize, float FOV, float FNumber)
    : Camera(camPos, camForward, filmSize) {
  // determine focal length
  focalLength = 0.5f * filmSize[0] / std::tan(0.5f * FOV);

  // set lens radius
  lensRadius = 2.0f * focalLength / FNumber;

  spdlog::info("[ThinLensCamera] focalLength: " + std::to_string(focalLength));
  spdlog::info("[ThinLensCamera] lensRadius: " + std::to_string(lensRadius));

  // init a, b
  // focus inf
  a = 10000.0f;
  b = focalLength;
}

Vec3 ThinLensCamera::We([[maybe_unused]] const Vec2& uv,
                        [[maybe_unused]] const Vec3& wi) const {
  const float cos = std::abs(dot(camForward, wi));
  return Vec3(b * b / (PI * lensRadius * lensRadius * std::pow(cos, 4.0f)));
}

void ThinLensCamera::focus(const Vec3& p) {
  // https://www.pbr-book.org/3ed-2018/Camera_Models/Realistic_Cameras#Focusing
  a = dot(p - camPos, camForward) - b;
  const float delta =
      0.5f * (a - b - std::sqrt(a + b) * std::sqrt(a + b - 4 * focalLength));
  // shift lens by delta
  b = b + delta;
  a = a - delta;

  spdlog::info("[ThinLensCamera] focusing at : " + p.toString());
  spdlog::info("[ThinLensCamera] a: " + std::to_string(a));
  spdlog::info("[ThinLensCamera] b: " + std::to_string(b));
}

bool ThinLensCamera::sampleRay(const Vec2& uv, Sampler& sampler, Ray& ray,
                               Vec3& wi, float& pdf) const {
  const Vec3 sensorPos = this->sensorPos(uv);
  const Vec3 lensCenter = camPos + b * camForward;

  // sample point on lens
  float pdf_area;
  const Vec2 pLens2D = sampleDisk(sampler.getNext2D(), lensRadius, pdf_area);
  const Vec3 pLens = lensCenter + pLens2D[0] * camRight + pLens2D[1] * camUp;
  Vec3 sensorToLens = normalize(pLens - sensorPos);

  // find intersection point with object plane
  const Vec3 sensorToLensCenter = normalize(lensCenter - sensorPos);
  const Vec3 pObject =
      sensorPos +
      ((a + b) / dot(sensorToLensCenter, camForward)) * sensorToLensCenter;

  ray = Ray(pLens, normalize(pObject - pLens));
  wi = sensorToLens;
  pdf = length2(pLens - sensorPos) / dot(sensorToLens, camForward) * pdf_area;
  return true;
}

}  // namespace LTRE