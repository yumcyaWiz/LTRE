#include "LTRE/bsdf/lambert.hpp"
#include "LTRE/camera/pinhole-camera.hpp"
#include "LTRE/core/model.hpp"
#include "LTRE/core/renderer.hpp"
#include "LTRE/core/scene.hpp"
#include "LTRE/integrator/pt.hpp"
#include "LTRE/intersector/bvh.hpp"
#include "LTRE/intersector/linear-intersector.hpp"
#include "LTRE/sampling/uniform.hpp"
#include "LTRE/shape/mesh.hpp"
#include "LTRE/shape/sphere.hpp"
#include "LTRE/sky/uniform-sky.hpp"

using namespace LTRE;

int main() {
  const int width = 512;
  const int height = 512;

  const auto intersector = std::make_shared<BVH<Primitive>>();
  const auto sky = std::make_shared<UniformSky>(Vec3(10));
  Scene scene(intersector, sky);

  // const auto sphere1 = std::make_shared<Sphere>(Vec3(0, -1001, 0), 1000);
  // const auto sphere2 = std::make_shared<Sphere>(Vec3(0), 1);
  // const auto mat1 = std::make_shared<Lambert>(Vec3(0.8));
  // const auto prim1 = Primitive(sphere1, mat1);
  // const auto prim2 = Primitive(sphere2, mat1);
  // scene.addPrimitive(prim1);
  // scene.addPrimitive(prim2);
  // scene.build();

  const auto model = Model("../assets/sponza/sponza.obj");
  // const auto model = Model("../assets/CornellBox/CornellBox-Original.obj");
  scene.addModel(model);
  scene.build();

  const auto camera =
      std::make_shared<PinholeCamera>(Vec3(0, 100, 0), Vec3(1, 0, 0));
  const auto integrator = std::make_shared<PT>();
  const auto sampler = std::make_shared<UniformSampler>();

  Renderer renderer(width, height, camera, integrator, sampler);
  renderer.render(scene, 100);
  renderer.writePPM("output.ppm");
}