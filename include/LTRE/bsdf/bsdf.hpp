#ifndef _LTRE_BSDF_H
#define _LTRE_BSDF_H
#include <memory>
#include <vector>

#include "LTRE/bsdf/bxdf/bxdf.hpp"
#include "LTRE/core/texture.hpp"
#include "LTRE/core/types.hpp"

namespace LTRE {

// TODO: handle BTDF case
class BSDF {
 private:
  static constexpr int MAX_NUM_BXDFS = 8;

  int nBxDF;
  std::shared_ptr<BxDF> bxdfs[MAX_NUM_BXDFS];
  float coefficients[MAX_NUM_BXDFS];

 public:
  BSDF() : nBxDF{0} {}

  void reset() { nBxDF = 0; }

  void add(const std::shared_ptr<BxDF>& bxdf, float coefficient) {
    if (nBxDF >= MAX_NUM_BXDFS) {
      spdlog::error("maximum number of BxDFs exceeded.");
      std::exit(EXIT_FAILURE);
    }

    bxdfs[nBxDF] = bxdf;
    coefficients[nBxDF] = coefficient;
    nBxDF++;
  }

  Vec3 f(const Vec3& wo, const Vec3& wi) const {
    Vec3 ret;
    for (int i = 0; i < nBxDF; ++i) {
      ret += coefficients[i] * bxdfs[i]->f(wo, wi);
    }
    return ret;
  }

  // TODO: use more nice sampling strategy
  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi, float& pdf) const {
    // choose 1 BxDF randomly
    int idx = sampler.getNext1D() * nBxDF;
    if (idx == nBxDF) idx--;

    // sample from that BxDF
    const Vec3 ret =
        coefficients[idx] * bxdfs[idx]->sample(sampler, wo, wi, pdf);
    pdf /= nBxDF;

    return ret;
  }
};

}  // namespace LTRE

#endif