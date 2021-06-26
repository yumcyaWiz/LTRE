#ifndef _LTRE_SCENE_H
#define _LTRE_SCENE_H
#include <memory>

#include "LTRE/core/model.hpp"
#include "LTRE/intersector/intersector.hpp"
#include "LTRE/sky/sky.hpp"

namespace LTRE {

class Scene {
 private:
  std::shared_ptr<Intersector<Primitive>> intersector;
  const std::shared_ptr<Sky> sky;

  std::vector<std::shared_ptr<Light>> lights;

 public:
  Scene() {}
  Scene(const std::shared_ptr<Intersector<Primitive>>& intersector,
        const std::shared_ptr<Sky>& sky)
      : intersector(intersector), sky(sky) {}

  void addPrimitive(const Primitive& primitive) {
    intersector->addPrimitive(primitive);
  }

  void addModel(const Model& model) {
    // for each meshes
    for (unsigned int i = 0; i < model.meshes.size(); ++i) {
      // create Primitive
      const std::shared_ptr<Mesh> shape = model.meshes[i];
      const auto material = model.createMaterial(i);
      const auto areaLight = model.createAreaLight(i);
      const Primitive prim = Primitive(shape, material, areaLight);

      // add Primitive to intersector
      intersector->addPrimitive(prim);
    }
  }

  void build() {
    // build intersector
    intersector->build();

    // initialize lights
    for (const auto& prim : intersector->primitives) {
      if (prim.hasArealight()) {
        lights.push_back(prim.getAreaLightPtr());
      }
    }
    spdlog::info("[Scene] number of lights: {}", lights.size());
  }

  bool intersect(const Ray& ray, IntersectInfo& info) const {
    return intersector->intersect(ray, info);
  }

  Vec3 getSkyRadiance(const Ray& ray) const { return sky->radiance(ray); }
};

}  // namespace LTRE

#endif