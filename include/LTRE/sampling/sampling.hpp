#ifndef _LTRE_SAMPLING_H
#define _LTRE_SAMPLING_H

#include <vector>

#include "LTRE/math/vec2.hpp"
#include "LTRE/math/vec3.hpp"

namespace LTRE {

Vec3 sampleHemisphere(const Vec2& uv, float& pdf);
float sampleHemispherePdf();

Vec3 sampleCosineHemisphere(const Vec2& uv, float& pdf);
float sampleCosineHemispherePdf(const Vec3& w);

Vec3 sampleSphere(const Vec2& uv, float& pdf);
float sampleSpherePdf();

Vec2 sampleDisk(const Vec2& uv, float R, float& pdf);
float sampleDiskPdf(float R);

Vec2 samplePlane(const Vec2& uv, float lx, float ly, float& pdf);
float samplePlanePdf(float lx, float ly);

class DiscreteEmpiricalDistribution1D {
 private:
  std::vector<float> cdf;

 public:
  DiscreteEmpiricalDistribution1D(const float* values, unsigned int N);
  DiscreteEmpiricalDistribution1D(const std::vector<float>& values);

  unsigned int sample(float u, float& pdf) const;
};

}  // namespace LTRE

#endif