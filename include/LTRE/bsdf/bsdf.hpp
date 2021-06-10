#ifndef _LTRE_BSDF_H
#define _LTRE_BSDF_H
#include <memory>
#include <vector>

#include "LTRE/bsdf/bxdf/bxdf.hpp"
#include "LTRE/core/intersect-info.hpp"
#include "LTRE/core/texture.hpp"

namespace LTRE {

// TODO: handle BTDF case
class BSDF {
 private:
  int nBxDF;
  std::shared_ptr<BxDF> bxdfs[8];

 public:
  BSDF() : nBxDF{0} {}

  void reset() { nBxDF = 0; }

  void add(const std::shared_ptr<BxDF>& bxdf) { bxdfs[nBxDF++] = bxdf; }

  Vec3 f(const Vec3& wo, const Vec3& wi) const {
    Vec3 ret;
    for (int i = 0; i < nBxDF; ++i) {
      ret += bxdfs[i]->f(wo, wi);
    }
    return ret;
  }

  // TODO: use more nice sampling strategy
  Vec3 sample(Sampler& sampler, const Vec3& wo, Vec3& wi, float& pdf) const {
    // choose 1 BxDF randomly
    int idx = sampler.getNext1D() * nBxDF;
    if (idx == nBxDF) idx--;

    // sample from that BxDF
    const Vec3 ret = bxdfs[idx]->sample(sampler, wo, wi, pdf);
    pdf /= nBxDF;

    return ret;
  }
};

}  // namespace LTRE

#endif