#ifndef _LTRE_CAMERA_H
#define _LTRE_CAMERA_H
#include "LTRE/core/ray.hpp"
#include "LTRE/math/vec3.hpp"

namespace LTRE {

class Camera {
 protected:
  Vec3 camPos;
  Vec3 camForward;
  Vec3 camRight;
  Vec3 camUp;

 public:
  Camera(const Vec3& camPos, const Vec3& camForward)
      : camPos(camPos), camForward(camForward) {
    camRight = normalize(cross(camForward, Vec3(0, 1, 0)));
    camUp = normalize(cross(camRight, camForward));
  }

  virtual bool sampleRay(float u, float v, Ray& ray, float& pdf) const = 0;
};

}  // namespace LTRE

#endif