#include "LTRE/sampling/sampling.hpp"

#include <algorithm>

#include "LTRE/core/constant.hpp"

namespace LTRE {

Vec3 sampleHemisphere(const Vec2& uv, float& pdf) {
  const float theta = std::acos(std::max(1.0f - uv[0], 0.0f));
  const float phi = PI_MUL_2 * uv[1];
  pdf = PI_MUL_2_INV;
  return sphericalToCartesian(theta, phi);
}
float sampleHemispherePdf() { return PI_MUL_2_INV; }

Vec3 sampleCosineHemisphere(const Vec2& uv, float& pdf) {
  const float theta =
      0.5f * std::acos(std::clamp(1.0f - 2.0f * uv[0], -1.0f, 1.0f));
  const float phi = PI_MUL_2 * uv[1];
  const float cosTheta = std::cos(theta);
  pdf = PI_INV * cosTheta;
  return sphericalToCartesian(theta, phi);
}
float sampleCosineHemispherePdf(const Vec3& w) { return PI_INV * w[1]; }

Vec3 sampleSphere(const Vec2& uv, float& pdf) {
  const float theta = std::acos(std::clamp(1.0f - 2.0f * uv[0], -1.0f, 1.0f));
  const float phi = PI_MUL_2 * uv[1];
  pdf = PI_MUL_4_INV;
  return sphericalToCartesian(theta, phi);
}
float sampleSpherePdf() { return PI_MUL_4_INV; }

Vec2 sampleDisk(const Vec2& uv, float R, float& pdf) {
  const float r = R * std::sqrt(std::max(uv[0], 0.0f));
  const float theta = PI_MUL_2 * uv[1];
  pdf = 1.0f / (R * R) * PI_INV;
  return Vec2(r * std::cos(theta), r * std::sin(theta));
}
float sampleDiskPdf(float R) { return 1.0f / (R * R) * PI_INV; }

Vec2 samplePlane(const Vec2& uv, float lx, float ly, float& pdf) {
  pdf = 1.0f / (lx * ly);
  return Vec2(uv[0] * lx, uv[1] * ly);
}
float samplePlanePdf(float lx, float ly) { return 1.0f / (lx * ly); }

DiscreteEmpiricalDistribution1D::DiscreteEmpiricalDistribution1D(
    const float* values, unsigned int N) {
  // sum f
  float sum = 0;
  for (std::size_t i = 0; i < N; ++i) {
    sum += values[i];
  }

  // compute cdf
  cdf.resize(N + 1);
  cdf[0] = 0;
  for (std::size_t i = 1; i < N + 1; ++i) {
    cdf[i] = cdf[i - 1] + values[i - 1] / sum;
  }
}

DiscreteEmpiricalDistribution1D::DiscreteEmpiricalDistribution1D(
    const std::vector<float>& values)
    : DiscreteEmpiricalDistribution1D(values.data(), values.size()) {}

unsigned int DiscreteEmpiricalDistribution1D::sample(float u,
                                                     float& pdf) const {
  // inverse cdf
  int x = std::lower_bound(cdf.begin(), cdf.end(), u) - cdf.begin();
  if (x == 0) {
    x++;
  }

  // compute pdf
  pdf = cdf[x] - cdf[x - 1];

  // NOTE: cdf's index is +1 from values
  return x - 1;
}

}  // namespace LTRE