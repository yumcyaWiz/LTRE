#include "LTRE/bsdf/bxdf/bxdf.hpp"

namespace LTRE {

float BxDF::cosTheta(const Vec3& w) { return w[1]; }
float BxDF::cos2Theta(const Vec3& w) { return w[1] * w[1]; }
float BxDF::absCosTheta(const Vec3& w) { return std::abs(w[1]); }

float BxDF::sin2Theta(const Vec3& w) {
  return std::max(0.0f, 1.0f - cos2Theta(w));
}
float BxDF::sinTheta(const Vec3& w) { return std::sqrt(sin2Theta(w)); }
float BxDF::absSinTheta(const Vec3& w) { return std::abs(sinTheta(w)); }

float BxDF::tanTheta(const Vec3& w) { return sinTheta(w) / cosTheta(w); }
float BxDF::tan2Theta(const Vec3& w) { return sin2Theta(w) / cos2Theta(w); }
float BxDF::absTanTheta(const Vec3& w) { return std::abs(tanTheta(w)); }

float BxDF::cosPhi(const Vec3& w) {
  float sin = sinTheta(w);
  return (sin == 0) ? 1 : std::clamp(w[0] / sin, -1.0f, 1.0f);
}
float BxDF::sinPhi(const Vec3& w) {
  float sin = sinTheta(w);
  return (sin == 0) ? 0 : std::clamp(w[2] / sin, -1.0f, 1.0f);
}
float BxDF::cos2Phi(const Vec3& w) { return cosPhi(w) * cosPhi(w); }
float BxDF::sin2Phi(const Vec3& w) { return sinPhi(w) * sinPhi(w); }

Vec3 BxDF::reflect(const Vec3& v, const Vec3& n) {
  return -v + 2.0f * dot(v, n) * n;
}

Vec3 BxDF::refract(const Vec3& v, const Vec3& n, float iorI, float iorT) {
  const Vec3 t_h = -iorI / iorT * (v - dot(v, n) * n);
  // total reflection
  if (length2(t_h) > 1.0f) {
    return reflect(v, n);
  }
  const Vec3 t_p = -std::sqrt(std::max(1.0f - length2(t_h), 0.0f)) * n;
  return t_h + t_p;
}

}  // namespace LTRE