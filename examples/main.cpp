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
#include "LTRE/light/sky/ibl.hpp"
#include "LTRE/light/sky/uniform-sky.hpp"
#include "LTRE/sampling/uniform.hpp"
#include "LTRE/shape/mesh.hpp"
#include "LTRE/shape/plane.hpp"
#include "LTRE/shape/sphere.hpp"

using namespace LTRE;

int main() {
  const int width = 512;
  const int height = 512;

  const auto intersector =
      std::make_shared<BVH<Primitive, BVHSplitStrategy::SAH>>();
  const auto sky = std::make_shared<IBL>("lythwood_field_4k.hdr", 0.5f);
  Scene scene(intersector, sky);

  const auto sphere2 = std::make_shared<Sphere>(Vec3(0), 1);
  const auto sphere3 = std::make_shared<Sphere>(Vec3(-3, 0, 0), 1);
  const auto sphere4 = std::make_shared<Sphere>(Vec3(3, 0, 0), 1);
  const auto plane =
      std::make_shared<Plane>(Vec3(-5, -1, -5), Vec3(0, 0, 10), Vec3(10, 0, 0));
  const auto tex1 = std::make_shared<UniformTexture<Vec3>>(Vec3(0.8));
  const auto tex2 = std::make_shared<UniformTexture<Vec3>>(Vec3(0.8, 0.2, 0.2));
  const auto tex3 = std::make_shared<UniformTexture<Vec3>>(Vec3(0.2, 0.8, 0.2));
  const auto tex4 = std::make_shared<UniformTexture<Vec3>>(Vec3(0.2, 0.2, 0.8));
  const auto mat1 = std::make_shared<Diffuse>(tex1, 0.2);
  const auto mat2 =
      std::make_shared<DisneyPrincipledBRDF>(tex2, 0, 0, 1, 0, 0, 0, 0, 0, 0);
  const auto mat3 = std::make_shared<Diffuse>(tex3, 0.2);
  const auto mat4 = std::make_shared<Diffuse>(tex4, 0.2);
  const auto prim1 = Primitive(plane, mat1);
  const auto prim2 = Primitive(sphere2, mat2);
  const auto prim3 = Primitive(sphere3, mat3);
  const auto prim4 = Primitive(sphere4, mat4);
  scene.addPrimitive(prim1);
  scene.addPrimitive(prim2);
  scene.addPrimitive(prim3);
  scene.addPrimitive(prim4);
  scene.build();

  // const auto model = Model("../assets/bunny/bunny.obj");
  // const auto model = Model("../assets/sponza/sponza.obj");
  // const auto model = Model("../assets/CornellBox/CornellBox-Original.obj");
  // const auto model = Model("../assets/salle_de_bain/salle_de_bain.obj");
  // const auto model = Model("../assets/test.obj");
  // scene.addModel(model);
  // scene.build();

  const auto camera = std::make_shared<ThinLensCamera>(
      Vec3(0, 1, 5), Vec3(0, 0, -1), Vec2(0.025, 0.025), PI / 2.0f);
  // const auto camera = std::make_shared<ThinLensCamera>(
  //     Vec3(-1000, 350, 0), Vec3(1, 0, 0), Vec2(0.025, 0.025), PI / 4.0f,
  //     0.4f);
  // const auto camera = std::make_shared<ThinLensCamera>(
  //     Vec3(0, 16, 60), Vec3(0, 0, -1), Vec2(0.025, 0.025), PI / 4.0f, 1.4f);
  const auto integrator = std::make_shared<PT>();
  const auto sampler = std::make_shared<UniformSampler>();

  Renderer renderer(width, height, camera, integrator, sampler);
  renderer.focus(scene);
  renderer.render(scene, 1000);
  renderer.writePPM("output.ppm", AOVType::BEAUTY);
}