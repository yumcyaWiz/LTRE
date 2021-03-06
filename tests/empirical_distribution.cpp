#include "LTRE/sampling/sampling.hpp"
#include "LTRE/sampling/uniform.hpp"
#include "gtest/gtest.h"
#include "spdlog/spdlog.h"

using namespace LTRE;

TEST(DiscreteEmpirical1D, BasicAssertions) {
  UniformSampler sampler(0);

  std::vector<float> values;
  values.push_back(1);
  values.push_back(1);
  values.push_back(1);
  values.push_back(1);

  DiscreteEmpiricalDistribution1D dist{values};

  const float u = 1.0f;
  float pdf;
  unsigned int x = dist.sample(u, pdf);
  spdlog::info("u: {0}, x: {1}, pdf: {2}", u, x, pdf);
}