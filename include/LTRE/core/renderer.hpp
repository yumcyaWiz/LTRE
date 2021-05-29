#ifndef _LTRE_RENDERER_H
#define _LTRE_RENDERER_H
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

    for (int j = 0; j < width; ++j) {
      for (int i = 0; i < height; ++i) {
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

  void writePPM(const std::string& filename) const { film.writePPM(filename); }
};

}  // namespace LTRE

#endif