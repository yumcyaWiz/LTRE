#ifndef _LTRE_RENDERER_H
#define _LTRE_RENDERER_H
#include <memory>

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

  AOV(unsigned int width, unsigned int height);
};

class Renderer {
 private:
  unsigned int width;
  unsigned int height;
  std::shared_ptr<Camera> camera;
  std::shared_ptr<Integrator> integrator;
  std::shared_ptr<Sampler> sampler;

  AOV aov;

  void renderFirstHitAOV(const Scene& scene);

 public:
  Renderer(unsigned int width, unsigned int height,
           const std::shared_ptr<Camera>& camera,
           const std::shared_ptr<Integrator>& integrator,
           const std::shared_ptr<Sampler>& sampler);

  // focus at specified point
  void focus(const Vec3& p);

  // focus at specified direction
  void focus(const Ray& ray, const Scene& scene);

  // focus at camera direction
  void focus(const Scene& scene);

  void render(const Scene& scene, unsigned int samples);

  // limitTime [ms]
  void renderWithInLimitTime(const Scene& scene, unsigned int limitTime);

  void writePPM(const std::filesystem::path& filepath, const AOVType& aovType);
};

}  // namespace LTRE

#endif