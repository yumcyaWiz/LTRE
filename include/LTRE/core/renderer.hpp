#ifndef _LTRE_RENDERER_H
#define _LTRE_RENDERER_H
#include <omp.h>

#include <chrono>
#include <memory>

#include "spdlog/spdlog.h"
//
#include "LTRE/camera/camera.hpp"
#include "LTRE/core/image.hpp"
#include "LTRE/core/scene.hpp"
#include "LTRE/integrator/integrator.hpp"
#include "LTRE/sampling/sampler.hpp"

namespace LTRE {

enum class AOVType {
  BEAUTY,
  POSITION,
  DEPTH,
  NORMAL,
  BARYCENTRIC,
  TEXCOORDS,
  BASECOLOR
};

struct AOV {
  Image<Vec3> beauty;
  Image<Vec3> position;
  Image<float> depth;
  Image<Vec3> normal;
  Image<Vec2> barycentric;
  Image<Vec2> texcoords;
  Image<Vec3> baseColor;

  AOV(unsigned int width, unsigned int height)
      : beauty{width, height},
        position{width, height},
        depth{width, height},
        normal{width, height},
        barycentric{width, height},
        texcoords{width, height},
        baseColor{width, height} {}
};

class Renderer {
 private:
  unsigned int width;
  unsigned int height;
  std::shared_ptr<Camera> camera;
  std::shared_ptr<Integrator> integrator;
  std::shared_ptr<Sampler> sampler;

  AOV aov;

 public:
  Renderer(unsigned int width, unsigned int height,
           const std::shared_ptr<Camera>& camera,
           const std::shared_ptr<Integrator>& integrator,
           const std::shared_ptr<Sampler>& sampler)
      : width(width),
        height(height),
        camera{camera},
        integrator{integrator},
        sampler{sampler},
        aov{width, height} {}

  // focus at specified point
  void focus(const Vec3& p) { camera->focus(p); }
  // focus at specified direction
  void focus(const Ray& ray, const Scene& scene) {
    IntersectInfo info;
    if (scene.intersect(ray, info)) {
      focus(info.surfaceInfo.position);
    }
  }
  // focus at camera direction
  void focus(const Scene& scene) {
    Ray ray(camera->getCameraPosition(), camera->getCameraForward());
    IntersectInfo info;
    if (scene.intersect(ray, info)) {
      focus(info.surfaceInfo.position);
    }
  }

  void render(const Scene& scene, int samples) {
    spdlog::info("[Renderer] samples: " + std::to_string(samples));

    spdlog::info("[Renderer] rendering started...");
    const auto startTime = std::chrono::steady_clock::now();
#pragma omp parallel for schedule(dynamic, 1) collapse(2)
    for (unsigned int j = 0; j < height; ++j) {
      for (unsigned int i = 0; i < width; ++i) {
        // setup sampler
        std::unique_ptr<Sampler> sampler = this->sampler->clone();
        sampler->setSeed(i + width * j);

        // set aov
        {
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
              aov.normal.setPixel(i, j,
                                  0.5f * (info.surfaceInfo.normal + 1.0f));
              aov.barycentric.setPixel(i, j, info.barycentric);
              aov.texcoords.setPixel(i, j, info.surfaceInfo.uv);
              aov.baseColor.setPixel(
                  i, j, info.hitPrimitive->baseColor(info.surfaceInfo));
            }
          }
        }

        // compute radiance
        Vec3 radiance(0);
        for (int k = 0; k < samples; ++k) {
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
            const float cos = std::abs(dot(wi, camera->getCameraForward()));

            // integrate light transport equation
            radiance +=
                We * integrator->integrate(ray, scene, *sampler) * cos / pdf;
          }
        }

        if (radiance.isNan()) {
          spdlog::error("[Renderer] radiance has NaN");
          std::exit(EXIT_FAILURE);
        }

        // take average
        radiance /= samples;
        aov.beauty.setPixel(i, j, radiance);
      }
    }
    const auto endTime = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        endTime - startTime)
                        .count();
    spdlog::info("[Renderer] rendering finished in " + std::to_string(ms) +
                 " ms");
  }

  void writePPM(const std::string& filename, const AOVType& aovType) {
    switch (aovType) {
      case AOVType::BEAUTY: {
        gammaCorrection(aov.beauty);
        aov.beauty.writePPM(filename);
        break;
      }
      case AOVType::POSITION: {
        aov.position.writePPM(filename);
        break;
      }
      case AOVType::DEPTH: {
        aov.depth.writePPM(filename);
        break;
      }
      case AOVType::NORMAL: {
        aov.normal.writePPM(filename);
        break;
      }
      case AOVType::BARYCENTRIC: {
        aov.barycentric.writePPM(filename);
        break;
      }
      case AOVType::TEXCOORDS: {
        aov.texcoords.writePPM(filename);
        break;
      }
      case AOVType::BASECOLOR: {
        gammaCorrection(aov.baseColor);
        aov.baseColor.writePPM(filename);
        break;
      }
    }
  }
};

}  // namespace LTRE

#endif