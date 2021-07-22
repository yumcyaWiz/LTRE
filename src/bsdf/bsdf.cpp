#include "LTRE/bsdf/bsdf.hpp"

#include "spdlog/spdlog.h"

namespace LTRE {

BSDF::BSDF() : nBxDF{0} {}

void BSDF::reset() { nBxDF = 0; }

void BSDF::add(const std::shared_ptr<BxDF>& bxdf, float coefficient) {
  if (nBxDF >= MAX_NUM_BXDFS) {
    spdlog::error("maximum number of BxDFs exceeded.");
    std::exit(EXIT_FAILURE);
  }

  bxdfs[nBxDF] = bxdf;
  coefficients[nBxDF] = coefficient;
  nBxDF++;
}

Vec3 BSDF::f(const Vec3& wo, const Vec3& wi) const {
  Vec3 ret;
  for (int i = 0; i < nBxDF; ++i) {
    ret += coefficients[i] * bxdfs[i]->f(wo, wi);
  }
  return ret;
}

// TODO: use more nice sampling strategy
Vec3 BSDF::sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
                  float& pdf) const {
  // choose 1 BxDF randomly
  int idx = sampler.getNext1D() * nBxDF;
  if (idx == nBxDF) idx--;

  // sample from that BxDF
  const Vec3 ret = coefficients[idx] * bxdfs[idx]->sample(sampler, wo, wi, pdf);
  pdf /= nBxDF;

  // pbrt method
  // NOTE: maybe wrong
  // for (int i = 0; i < nBxDF; ++i) {
  //   if (i != idx) {
  //     pdf += bxdfs[i]->pdf(wo, wi);
  //   }
  // }
  // pdf /= nBxDF;

  return ret;
}

}  // namespace LTRE