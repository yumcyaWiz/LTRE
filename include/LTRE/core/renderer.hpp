#ifndef _LTRE_RENDERER_H
#define _LTRE_RENDERER_H
#include <omp.h>

#include <memory>

#include "LTRE/camera/camera.hpp"
#include "LTRE/core/film.hpp"
#include "LTRE/core/scene.hpp"
#include "LTRE/integrator/integrator.hpp"
#include "LTRE/sampling/sampler.hpp"

namespace LTRE {

class Renderer {
 private:
  Film film;
  std::shared_ptr<Camera> camera;
  std::shared_ptr<Integrator> integrator;
  std::shared_ptr<Sampler> sampler;

 public:
  Renderer(unsigned int width, unsigned int height,
           const std::shared_ptr<Camera>& camera,
           const std::shared_ptr<Integrator>& integrator,
           const std::shared_ptr<Sampler>& sampler)
      : film{width, height},
        camera{camera},
        integrator{integrator},
        sampler{sampler} {}

  void render(const Scene& scene, int samples) {
    unsigned int width = film.getWidth();
    unsigned int height = film.getHeight();

#pragma omp parallel for schedule(dynamic, 1) collapse(2)
    for (unsigned int j = 0; j < width; ++j) {
      for (unsigned int i = 0; i < height; ++i) {
        // setup sampler
        std::unique_ptr<Sampler> sampler = this->sampler->clone();
        sampler->setSeed(i + width * j);

        Vec3 radiance(0);
        for (int k = 0; k < samples; ++k) {
          // compute (u, v) with SSAA
          Vec2 uv;
          uv[0] = (2.0f * (i + sampler->getNext1D()) - width) / width;
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
        film.setPixel(i, j, radiance);
      }
    }
  }

  void renderNormal(const Scene& scene) {
    unsigned int width = film.getWidth();
    unsigned int height = film.getHeight();

#pragma omp parallel for schedule(dynamic, 1) collapse(2)
    for (unsigned int j = 0; j < width; ++j) {
      for (unsigned int i = 0; i < height; ++i) {
        // setup sampler
        std::unique_ptr<Sampler> sampler = this->sampler->clone();
        sampler->setSeed(i + width * j);

        Vec3 radiance(0);
        // compute (u, v) with SSAA
        Vec2 uv;
        uv[0] = (2.0f * (i + sampler->getNext1D()) - width) / width;
        uv[1] = (2.0f * (j + sampler->getNext1D()) - height) / height;

        // generate camera ray
        Ray ray;
        float pdf;
        if (camera->sampleRay(uv, *sampler, ray, pdf)) {
          // set normal color
          IntersectInfo info;
          if (scene.intersect(ray, info)) {
            radiance = 0.5f * (info.hitNormal + 1.0f);
          }
        }
        film.setPixel(i, j, radiance);
      }
    }
  }

  void renderUV(const Scene& scene) {
    unsigned int width = film.getWidth();
    unsigned int height = film.getHeight();

#pragma omp parallel for schedule(dynamic, 1) collapse(2)
    for (unsigned int j = 0; j < width; ++j) {
      for (unsigned int i = 0; i < height; ++i) {
        // setup sampler
        std::unique_ptr<Sampler> sampler = this->sampler->clone();
        sampler->setSeed(i + width * j);

        Vec3 radiance(0);
        // compute (u, v) with SSAA
        Vec2 uv;
        uv[0] = (2.0f * (i + sampler->getNext1D()) - width) / width;
        uv[1] = (2.0f * (j + sampler->getNext1D()) - height) / height;

        // generate camera ray
        Ray ray;
        float pdf;
        if (camera->sampleRay(uv, *sampler, ray, pdf)) {
          // set uv color
          IntersectInfo info;
          if (scene.intersect(ray, info)) {
            radiance = Vec3(info.uv[0], info.uv[1], 0.0f);
          }
        }
        film.setPixel(i, j, radiance);
      }
    }
  }

  void renderBaseColor(const Scene& scene) {
    unsigned int width = film.getWidth();
    unsigned int height = film.getHeight();

#pragma omp parallel for schedule(dynamic, 1) collapse(2)
    for (unsigned int j = 0; j < width; ++j) {
      for (unsigned int i = 0; i < height; ++i) {
        // setup sampler
        std::unique_ptr<Sampler> sampler = this->sampler->clone();
        sampler->setSeed(i + width * j);

        Vec3 radiance(0);
        // compute (u, v) with SSAA
        Vec2 uv;
        uv[0] = (2.0f * (i + sampler->getNext1D()) - width) / width;
        uv[1] = (2.0f * (j + sampler->getNext1D()) - height) / height;

        // generate camera ray
        Ray ray;
        float pdf;
        if (camera->sampleRay(uv, *sampler, ray, pdf)) {
          // set diffuse color
          IntersectInfo info;
          if (scene.intersect(ray, info)) {
            radiance = info.hitPrimitive->bsdf->baseColor(info);
          }
        }
        film.setPixel(i, j, radiance);
      }
    }
  }

  void writePPM(const std::string& filename) const {
    film.gammaCorrection();
    film.writePPM(filename);
  }
};

}  // namespace LTRE

#endif