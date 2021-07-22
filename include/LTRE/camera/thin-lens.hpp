#ifndef _LTRE_THIN_LENS_H
#define _LTRE_THIN_LENS_H
#include "LTRE/camera/camera.hpp"

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
                 float FOV = PI_MUL_2_INV, float FNumber = 5.6f);

  Vec3 We(const Vec2& uv, const Vec3& wi) const override;
  void focus(const Vec3& p) override;
  bool sampleRay(const Vec2& uv, Sampler& sampler, Ray& ray, Vec3& wi,
                 float& pdf) const override;
};

}  // namespace LTRE

#endif