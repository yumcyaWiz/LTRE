#include "LTRE/bsdf/bxdf/microfacet.hpp"
#include "LTRE/camera/pinhole-camera.hpp"
#include "LTRE/camera/thin-lens.hpp"
#include "LTRE/core/model.hpp"
#include "LTRE/core/renderer.hpp"
#include "LTRE/core/scene.hpp"
#include "LTRE/integrator/ao.hpp"
#include "LTRE/integrator/nee.hpp"
#include "LTRE/integrator/pt.hpp"
#include "LTRE/intersector/bvh.hpp"
#include "LTRE/light/area-light.hpp"
#include "LTRE/light/sky/uniform-sky.hpp"
#include "LTRE/sampling/uniform.hpp"
#include "LTRE/shape/mesh.hpp"
#include "LTRE/shape/sphere.hpp"

using namespace LTRE;

int main() {
  const int width = 512;
  const int height = 512;

  const auto intersector =
      std::make_shared<BVH<Primitive, BVHSplitStrategy::SAH>>();
  const auto sky = std::make_shared<UniformSky>(Vec3(1));
  Scene scene(intersector, sky);

  // const auto sphere1 = std::make_shared<Sphere>(Vec3(0, -1001, 0), 1000);
  // const auto sphere2 = std::make_shared<Sphere>(Vec3(0), 1);
  // const auto sphere3 = std::make_shared<Sphere>(Vec3(-2, 0, 0), 1);
  // const auto sphere4 = std::make_shared<Sphere>(Vec3(2, 0, 0), 1);
  // const auto tex1 = std::make_shared<UniformTexture<Vec3>>(Vec3(0.8));
  // const auto tex2 = std::make_shared<UniformTexture<Vec3>>(Vec3(0.8, 0.2,
  // 0.2)); const auto tex3 = std::make_shared<UniformTexture<Vec3>>(Vec3(0.2,
  // 0.8, 0.2)); const auto tex4 =
  // std::make_shared<UniformTexture<Vec3>>(Vec3(0.2, 0.2, 0.8)); const auto
  // mat1 = std::make_shared<Lambert>(tex1); const auto mat2 =
  // std::make_shared<Lambert>(tex2); const auto mat3 =
  // std::make_shared<Lambert>(tex3); const auto mat4 =
  // std::make_shared<Lambert>(tex4); const auto prim1 = Primitive(sphere1,
  // mat1); const auto prim2 = Primitive(sphere2, mat2); const auto prim3 =
  // Primitive(sphere3, mat3); const auto prim4 = Primitive(sphere4, mat4);
  // scene.addPrimitive(prim1);
  // scene.addPrimitive(prim2);
  // scene.addPrimitive(prim3);
  // scene.addPrimitive(prim4);
  // scene.build();

  // const auto model = Model("../assets/bunny/bunny.obj");
  // const auto model = Model("../assets/sponza/sponza.obj");
  const auto model = Model("../assets/CornellBox/CornellBox-Original.obj");
  // const auto model = Model("../assets/salle_de_bain/salle_de_bain.obj");
  // const auto model = Model("../assets/test.obj");
  scene.addModel(model);
  scene.build();

  const auto camera = std::make_shared<PinholeCamera>(
      Vec3(0, 1, 5), Vec3(0, 0, -1), Vec2(0.025, 0.025), PI / 4.0f);
  // const auto camera = std::make_shared<ThinLensCamera>(
  //     Vec3(-1000, 350, 0), Vec3(1, 0, 0), Vec2(0.025, 0.025), PI / 4.0f,
  //     0.4f);
  // const auto camera = std::make_shared<ThinLensCamera>(
  //     Vec3(0, 16, 60), Vec3(0, 0, -1), Vec2(0.025, 0.025), PI / 4.0f, 1.4f);
  const auto integrator = std::make_shared<NEE>();
  const auto sampler = std::make_shared<UniformSampler>();

  Renderer renderer(width, height, camera, integrator, sampler);
  renderer.focus(scene);
  renderer.render(scene, 100);
  renderer.writePPM("output.ppm", AOVType::BEAUTY);
}