#ifndef _LTRE_BSDF_H
#define _LTRE_BSDF_H
#include <memory>

#include "LTRE/bsdf/bxdf/bxdf.hpp"

namespace LTRE {

// TODO: handle BTDF case
class BSDF {
 private:
  static constexpr int MAX_NUM_BXDFS = 8;

  int nBxDF;
  std::shared_ptr<BxDF> bxdfs[MAX_NUM_BXDFS];
  float coefficients[MAX_NUM_BXDFS];

 public:
  BSDF();

  void reset();

  void add(const std::shared_ptr<BxDF>& bxdf, float coefficient);

  Vec3 f(const Vec3& wo, const Vec3& wi) const;
  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi, float& pdf) const;
};

}  // namespace LTRE

#endif