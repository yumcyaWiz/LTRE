#include "LTRE/bsdf/bsdf.hpp"

#include "LTRE/sampling/sampling.hpp"

//
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

Vec3 BSDF::sample(Sampler& sampler, const Vec3& wo, Vec3& wi,
                  float& pdf) const {
  // construct EmpiricalDistribution1D with coef x reflectance
  std::vector<float> values(nBxDF);
  for (int i = 0; i < nBxDF; ++i) {
    values[i] = coefficients[i] * bxdfs[i]->reflectance(wo);
  }
  DiscreteEmpiricalDistribution1D dist(values);

  // choose 1 BxDF
  float pdf_choose_bxdf;
  const unsigned int idx = dist.sample(sampler.getNext1D(), pdf_choose_bxdf);

  // sample from that BxDF
  const Vec3 bxdf =
      coefficients[idx] * bxdfs[idx]->sample(sampler, wo, wi, pdf);
  pdf *= pdf_choose_bxdf;

  return bxdf;
}

}  // namespace LTRE