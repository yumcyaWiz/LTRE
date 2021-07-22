#include "LTRE/core/renderer.hpp"

#include <omp.h>

#include <chrono>

#include "spdlog/spdlog.h"
//
#include "LTRE/core/io.hpp"

namespace LTRE {

AOV::AOV(unsigned int width, unsigned int height)
    : beauty{width, height},
      position{width, height},
      depth{width, height},
      normal{width, height},
      barycentric{width, height},
      texcoords{width, height},
      baseColor{width, height} {}

void Renderer::renderFirstHitAOV(const Scene& scene) {
#pragma omp parallel for schedule(dynamic, 1) collapse(2)
  for (unsigned int j = 0; j < height; ++j) {
    for (unsigned int i = 0; i < width; ++i) {
      // setup sampler
      std::unique_ptr<Sampler> sampler = this->sampler->clone();
      sampler->setSeed(i + width * j);

      // compute (u, v)
      // NOTE: adding "-"" to flip uv
      Vec2 uv;
      uv[0] = -(2.0f * i - width) / height;
      uv[1] = -(height - 2.0f * j) / height;

      Ray ray;
      Vec3 wi;
      float pdf;
      if (camera->sampleRay(uv, *sampler, ray, wi, pdf)) {
        IntersectInfo info;
        if (scene.intersect(ray, info)) {
          aov.depth.setPixel(i, j, info.t);
          aov.position.setPixel(i, j, info.surfaceInfo.position);
          aov.normal.setPixel(i, j, 0.5f * (info.surfaceInfo.normal + 1.0f));
          aov.barycentric.setPixel(i, j, info.barycentric);
          aov.texcoords.setPixel(i, j, info.surfaceInfo.uv);
          aov.baseColor.setPixel(
              i, j, info.hitPrimitive->baseColor(info.surfaceInfo));
        }
      }
    }
  }
}

Renderer::Renderer(unsigned int width, unsigned int height,
                   const std::shared_ptr<Camera>& camera,
                   const std::shared_ptr<Integrator>& integrator,
                   const std::shared_ptr<Sampler>& sampler)
    : width(width),
      height(height),
      camera{camera},
      integrator{integrator},
      sampler{sampler},
      aov{width, height} {}

void Renderer::focus(const Vec3& p) { camera->focus(p); }

void Renderer::focus(const Ray& ray, const Scene& scene) {
  IntersectInfo info;
  if (scene.intersect(ray, info)) {
    focus(info.surfaceInfo.position);
  }
}

void Renderer::focus(const Scene& scene) {
  Ray ray(camera->getCameraPosition(), camera->getCameraForward());
  IntersectInfo info;
  if (scene.intersect(ray, info)) {
    focus(info.surfaceInfo.position);
  }
}

void Renderer::render(const Scene& scene, unsigned int samples) {
  renderFirstHitAOV(scene);

  spdlog::info("[Renderer] samples: " + std::to_string(samples));
  spdlog::info("[Renderer] rendering started...");

  const auto startTime = std::chrono::steady_clock::now();
#pragma omp parallel for schedule(dynamic, 1) collapse(2)
  for (unsigned int j = 0; j < height; ++j) {
    for (unsigned int i = 0; i < width; ++i) {
      // setup sampler
      std::unique_ptr<Sampler> sampler = this->sampler->clone();
      sampler->setSeed(i + width * j);

      // compute radiance
      Vec3 radiance(0);
      for (unsigned int sample = 0; sample < samples; ++sample) {
        // compute (u, v) with SSAA
        // NOTE: adding "-"" to flip uv
        Vec2 uv;
        uv[0] = -(2.0f * (i + sampler->getNext1D()) - width) / height;
        uv[1] = -(height - 2.0f * (j + sampler->getNext1D())) / height;

        // generate camera ray
        Ray ray;
        Vec3 wi;
        float pdf;
        if (camera->sampleRay(uv, *sampler, ray, wi, pdf)) {
          // evaluate We
          const Vec3 We = camera->We(uv, wi);

          // evaluate cos
          const float cos = std::max(dot(wi, camera->getCameraForward()), 0.0f);

          // integrate light transport equation
          const Vec3 dPhi =
              We * integrator->integrate(ray, scene, *sampler) * cos / pdf;
          if (dPhi.isNan()) {
            spdlog::error("[Renderer] radiance has NaN");
            continue;
            // std::exit(EXIT_FAILURE);
          }

          radiance += dPhi;
        }
      }

      // take average
      radiance /= samples;
      aov.beauty.setPixel(i, j, radiance);
    }
  }
  const auto endTime = std::chrono::steady_clock::now();
  const auto ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
          .count();
  spdlog::info("[Renderer] rendering finished in " + std::to_string(ms) +
               " ms");
}

void Renderer::renderWithInLimitTime(const Scene& scene,
                                     unsigned int limitTime) {
  renderFirstHitAOV(scene);

  spdlog::info("[Renderer] rendering started...");
  const auto startTime = std::chrono::steady_clock::now();

  // setup sampler
  std::vector<std::unique_ptr<Sampler>> samplers(width * height);
  for (unsigned int j = 0; j < height; ++j) {
    for (unsigned int i = 0; i < width; ++i) {
      samplers[i + width * j] = this->sampler->clone();
      samplers[i + width * j]->setSeed(i + width * j);
    }
  }

  // render beauty
  unsigned int nSamples = 0;
  for (unsigned int samples = 0; true; samples++) {
    // exit rendering if elapsed time is bigger than limit time
    const auto elapsedTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime)
            .count();
    if (elapsedTime >= limitTime) {
      break;
    }

    nSamples++;
#pragma omp parallel for schedule(dynamic, 1) collapse(2)
    for (unsigned int j = 0; j < height; ++j) {
      for (unsigned int i = 0; i < width; ++i) {
        const std::unique_ptr<Sampler>& sampler = samplers[i + width * j];

        // compute (u, v) with SSAA
        // NOTE: adding "-"" to flip uv
        Vec2 uv;
        uv[0] = -(2.0f * (i + sampler->getNext1D()) - width) / height;
        uv[1] = -(height - 2.0f * (j + sampler->getNext1D())) / height;

        // generate camera ray
        Ray ray;
        Vec3 wi;
        float pdf;
        if (camera->sampleRay(uv, *sampler, ray, wi, pdf)) {
          // evaluate We
          const Vec3 We = camera->We(uv, wi);

          // evaluate cos
          const float cos = std::max(dot(wi, camera->getCameraForward()), 0.0f);

          // integrate light transport equation
          const Vec3 radiance =
              We * integrator->integrate(ray, scene, *sampler) * cos / pdf;
          if (radiance.isNan()) {
            spdlog::error("[Renderer] radiance has NaN");
            continue;
            // std::exit(EXIT_FAILURE);
          }

          // accumulate radiance
          aov.beauty.addPixel(i, j, radiance);
        }
      }
    }
  }
  aov.beauty /= Vec3(nSamples);

  const auto elapsedTime =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::steady_clock::now() - startTime)
          .count();
  spdlog::info("[Renderer] rendering finished in {0} ms", elapsedTime);
  spdlog::info("[Renderer] samples: {0}", nSamples);
}

void Renderer::writePPM(const std::filesystem::path& filepath,
                        const AOVType& aovType) {
  switch (aovType) {
    case AOVType::BEAUTY: {
      gammaCorrection(aov.beauty);
      ImageWriter::writeImage(aov.beauty, filepath);
      break;
    }
    case AOVType::POSITION: {
      ImageWriter::writeImage(aov.position, filepath);
      break;
    }
    case AOVType::DEPTH: {
      ImageWriter::writeImage(aov.depth, filepath);
      break;
    }
    case AOVType::NORMAL: {
      ImageWriter::writeImage(aov.normal, filepath);
      break;
    }
    case AOVType::BARYCENTRIC: {
      ImageWriter::writeImage(aov.barycentric, filepath);
      break;
    }
    case AOVType::TEXCOORDS: {
      ImageWriter::writeImage(aov.texcoords, filepath);
      break;
    }
    case AOVType::BASECOLOR: {
      gammaCorrection(aov.baseColor);
      ImageWriter::writeImage(aov.baseColor, filepath);
      break;
    }
  }
}

}  // namespace LTRE