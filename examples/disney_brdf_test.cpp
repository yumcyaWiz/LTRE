#include "LTRE/camera/thin-lens.hpp"
#include "LTRE/core/material.hpp"
#include "LTRE/core/renderer.hpp"
#include "LTRE/core/scene.hpp"
#include "LTRE/core/texture.hpp"
#include "LTRE/integrator/nee.hpp"
#include "LTRE/integrator/pt.hpp"
#include "LTRE/intersector/bvh.hpp"
#include "LTRE/light/area-light.hpp"
#include "LTRE/light/sky/ibl.hpp"
#include "LTRE/light/sky/uniform-sky.hpp"
#include "LTRE/sampling/uniform.hpp"
#include "LTRE/shape/plane.hpp"
#include "LTRE/shape/sphere.hpp"

using namespace LTRE;

int main() {
  const int width = 512;
  const int height = 512;
  const int n_spheres = 10;
  const float radius = 1;

  const auto intersector =
      std::make_shared<BVH<Primitive, BVHSplitStrategy::SAH>>();
  const auto sky =
      std::make_shared<IBL>("entrance_hall_4k.hdr", 0, PI_DIV_2, 0.5f);
  Scene scene(intersector, sky);

  //   const auto floor_shape = std::make_shared<Plane>(
  //       Vec3(-15, -radius, -15), Vec3(0, 0, 30), Vec3(30, 0, 0));
  //   const auto floor_tex =
  //   std::make_shared<UniformTexture<Vec3>>(Vec3(0.9f)); const auto
  //   floor_material = std::make_shared<Diffuse>(floor_tex, 0.2f);
  //   scene.addPrimitive(Primitive(floor_shape, floor_material));

  //   const auto light_shape =
  //       std::make_shared<Plane>(Vec3(-5, 20, -5), Vec3(10, 0, 0), Vec3(0, 0,
  //       10));
  //   const auto light_ke = std::make_shared<UniformTexture<Vec3>>(Vec3(0.9f));
  //   const auto light_le =
  //   std::make_shared<UniformTexture<Vec3>>(Vec3(15.0f)); const auto light =
  //   std::make_shared<AreaLight>(light_le, light_shape);
  //   scene.addPrimitive(Primitive(light_shape, floor_material, light));

  // prepare shapes
  std::vector<std::shared_ptr<Shape>> shapes;
  for (int j = 0; j < 9; ++j) {
    for (int i = 0; i < n_spheres; ++i) {
      const float u = (2.0f * (i + 0.5f) - n_spheres) / n_spheres;
      const float v = (2.0f * (j + 0.5f) - 9) / 9;
      const float x = n_spheres * 1.2f * radius * u;
      const float y = 9.0f * 1.2f * radius * v;
      const auto shape = std::make_shared<Sphere>(Vec3(x, y, 0), radius);
      shapes.push_back(shape);
    }
  }

  // prepare texture
  std::vector<std::shared_ptr<Texture<Vec3>>> textures;
  textures.push_back(std::make_shared<UniformTexture<Vec3>>(Vec3(0.9f)));
  textures.push_back(
      std::make_shared<UniformTexture<Vec3>>(Vec3(0.9f, 0.9f, 0)));
  textures.push_back(std::make_shared<UniformTexture<Vec3>>(Vec3(0.9f, 0, 0)));
  textures.push_back(std::make_shared<UniformTexture<Vec3>>(Vec3(0.9f, 0, 0)));
  textures.push_back(std::make_shared<UniformTexture<Vec3>>(Vec3(0, 0, 0.9f)));
  textures.push_back(
      std::make_shared<UniformTexture<Vec3>>(Vec3(0.18f, 0.05f, 0.01f)));
  textures.push_back(
      std::make_shared<UniformTexture<Vec3>>(Vec3(0.18f, 0.05f, 0.01f)));
  textures.push_back(
      std::make_shared<UniformTexture<Vec3>>(Vec3(0, 0.095f, 0.184f)));
  textures.push_back(
      std::make_shared<UniformTexture<Vec3>>(Vec3(0, 0.095f, 0.184f)));

  // prepare materials
  std::vector<std::shared_ptr<Material>> materials;
  // subsurface
  for (int i = 0; i < n_spheres; ++i) {
    const float u = (i + 0.5f) / n_spheres;
    const auto material = std::make_shared<DisneyPrincipledBRDF>(
        textures[0], 1, u, 0, 0, 0, 0, 0, 0, 0);
    materials.push_back(material);
  }
  // metallic
  for (int i = 0; i < n_spheres; ++i) {
    const float u = (i + 0.5f) / n_spheres;
    const auto material = std::make_shared<DisneyPrincipledBRDF>(
        textures[1], 0, 0, u, 0, 0, 0, 0, 0, 0);
    materials.push_back(material);
  }
  // specular
  for (int i = 0; i < n_spheres; ++i) {
    const float u = (i + 0.5f) / n_spheres;
    const auto material = std::make_shared<DisneyPrincipledBRDF>(
        textures[2], 0, 0, 0, 0, 0, u, 0, 0, 0);
    materials.push_back(material);
  }
  // specularTint
  for (int i = 0; i < n_spheres; ++i) {
    const float u = (i + 0.5f) / n_spheres;
    const auto material = std::make_shared<DisneyPrincipledBRDF>(
        textures[3], 0, 0, 0, 0, 0, 1, u, 0, 0);
    materials.push_back(material);
  }
  // roughness
  for (int i = 0; i < n_spheres; ++i) {
    const float u = (i + 0.5f) / n_spheres;
    const auto material = std::make_shared<DisneyPrincipledBRDF>(
        textures[4], u, 0, 0, 0, 0, 1, 0, 0, 0);
    materials.push_back(material);
  }
  // sheen
  for (int i = 0; i < n_spheres; ++i) {
    const float u = (i + 0.5f) / n_spheres;
    const auto material = std::make_shared<DisneyPrincipledBRDF>(
        textures[5], 1, 0, 0, u, 0, 0, 0, 0, 0);
    materials.push_back(material);
  }
  // sheenTint
  for (int i = 0; i < n_spheres; ++i) {
    const float u = (i + 0.5f) / n_spheres;
    const auto material = std::make_shared<DisneyPrincipledBRDF>(
        textures[6], 1, 0, 0, 1, u, 0, 0, 0, 0);
    materials.push_back(material);
  }
  // clearcoat
  for (int i = 0; i < n_spheres; ++i) {
    const float u = (i + 0.5f) / n_spheres;
    const auto material = std::make_shared<DisneyPrincipledBRDF>(
        textures[7], 1, 0, 0, 0, 0, 0, 0, u, 1);
    materials.push_back(material);
  }
  // clearcoatGloss
  for (int i = 0; i < n_spheres; ++i) {
    const float u = (i + 0.5f) / n_spheres;
    const auto material = std::make_shared<DisneyPrincipledBRDF>(
        textures[8], 1, 0, 0, 0, 0, 0, 0, 1, u);
    materials.push_back(material);
  }

  // parepare primitives
  for (int j = 0; j < 9; ++j) {
    for (int i = 0; i < n_spheres; ++i) {
      const int idx = i + n_spheres * j;
      const auto shape = shapes[idx];
      const auto material = materials[idx];
      const auto primitive = Primitive(shape, material);
      scene.addPrimitive(primitive);
    }
  }
  scene.build();

  const auto camera = std::make_shared<ThinLensCamera>(
      Vec3(0, 0, -35), Vec3(0, 0, 1), Vec2(1, 1), PI / 4.0f);
  const auto integrator = std::make_shared<PT>();
  const auto sampler = std::make_shared<UniformSampler>();

  Renderer renderer(width, height, camera, integrator, sampler);
  renderer.focus(Vec3(0));
  renderer.render(scene, 100000);
  renderer.writePPM("output.ppm", AOVType::BEAUTY);
}