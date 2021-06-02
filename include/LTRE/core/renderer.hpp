#ifndef _LTRE_RENDERER_H
#define _LTRE_RENDERER_H
#include <omp.h>

#include <memory>

#include "LTRE/camera/camera.hpp"
#include "LTRE/core/image.hpp"
#include "LTRE/core/scene.hpp"
#include "LTRE/integrator/integrator.hpp"
#include "LTRE/sampling/sampler.hpp"

namespace LTRE {

enum class AOVType { BEAUTY, POSITION, DEPTH, NORMAL, BARYCENTRIC, TEXCOORDS };

struct AOV {
  Image<Vec3> beauty;
  Image<Vec3> position;
  Image<float> depth;
  Image<Vec3> normal;
  Image<Vec2> barycentric;
  Image<Vec2> texcoords;

  AOV(unsigned int width, unsigned int height)
      : beauty{width, height},
        position{width, height},
        depth{width, height},
        normal{width, height},
        barycentric{width, height},
        texcoords{width, height} {}
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

  void render(const Scene& scene, int samples) {
#pragma omp parallel for schedule(dynamic, 1) collapse(2)
    for (unsigned int j = 0; j < width; ++j) {
      for (unsigned int i = 0; i < height; ++i) {
        // setup sampler
        std::unique_ptr<Sampler> sampler = this->sampler->clone();
        sampler->setSeed(i + width * j);

        // set aov
        {
          Vec2 uv;
          uv[0] = (2.0f * i - width) / height;
          uv[1] = (2.0f * j - height) / height;
          Ray ray;
          float pdf;
          if (camera->sampleRay(uv, *sampler, ray, pdf)) {
            IntersectInfo info;
            if (scene.intersect(ray, info)) {
              aov.depth.setPixel(i, j, info.t);
              aov.position.setPixel(i, j, info.hitPos);
              aov.normal.setPixel(i, j, 0.5f * (info.hitNormal + 1.0f));
              aov.barycentric.setPixel(i, j, info.barycentric);
              aov.texcoords.setPixel(i, j, info.uv);
            }
          }
        }

        // compute radiance
        Vec3 radiance(0);
        for (int k = 0; k < samples; ++k) {
          // compute (u, v) with SSAA
          Vec2 uv;
          uv[0] = (2.0f * (i + sampler->getNext1D()) - width) / height;
          uv[1] = (2.0f * (j + sampler->getNext1D()) - height) / height;

          // generate camera ray
          Ray ray;
          float pdf;
          if (camera->sampleRay(uv, *sampler, ray, pdf)) {
            // integrate light transport equation
            radiance += integrator->integrate(ray, scene, *sampler) / pdf;
          }
        }

        // take average
        radiance /= samples;
        aov.beauty.setPixel(i, j, radiance);
      }
    }
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
    }
  }
};

}  // namespace LTRE

#endif