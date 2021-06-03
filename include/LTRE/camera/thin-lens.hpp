#ifndef _LTRE_THIN_LENS_H
#define _LTRE_THIN_LENS_H
#include "LTRE/camera/camera.hpp"
#include "LTRE/sampling/sampling.hpp"

namespace LTRE {
class ThinLensCamera : public Camera {
 private:
  float a;  // distance from lens to object plane
  float b;  // distance from lens to image plane
  float focalLength;
  float lensRadius;

 public:
  ThinLensCamera(const Vec3& camPos, const Vec3& camForward,
                 const Vec2& filmSize = Vec2(0.025, 0.025),
                 float FOV = PI_MUL_2_INV, float FNumber = 5.6f)
      : Camera(camPos, camForward, filmSize) {
    // determine focal length
    focalLength = 0.5f * filmSize[0] / std::tan(0.5f * FOV);

    // set lens radius
    lensRadius = focalLength / FNumber;

    // focus inf
    focus(camPos + 1e9f * camForward);
  }

  Vec3 We([[maybe_unused]] const Vec2& uv,
          [[maybe_unused]] const Vec3& wi) const override {
    return Vec3(b * b / (lensRadius * lensRadius));
  }

  void focus(const Vec3& p) override {
    // NOTE: lens equation
    a = dot(p - camPos, camForward) - b;
    b = 1.0f / (1.0f / focalLength - 1.0f / a);
  }

  bool sampleRay(const Vec2& uv, Sampler& sampler, Ray& ray,
                 float& pdf) const override {
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
    pdf = length2(pLens - sensorPos) / dot(sensorToLens, camForward) * pdf_area;
    return true;
  }
};

}  // namespace LTRE

#endif