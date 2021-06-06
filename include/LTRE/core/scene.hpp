#ifndef _LTRE_SCENE_H
#define _LTRE_SCENE_H
#include <memory>

#include "LTRE/bsdf/diffuse.hpp"
#include "LTRE/core/model.hpp"
#include "LTRE/intersector/intersector.hpp"
#include "LTRE/sky/sky.hpp"

namespace LTRE {

class Scene {
 private:
  std::shared_ptr<Intersector<Primitive>> intersector;
  const std::shared_ptr<Sky> sky;

 public:
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
      const auto bsdf = model.createBSDF(i);
      const auto areaLight = model.createAreaLight(i);
      const Primitive prim = Primitive(shape, bsdf, areaLight);

      // add Primitive to intersector
      intersector->addPrimitive(prim);
    }
  }

  void build() { intersector->build(); }

  bool intersect(const Ray& ray, IntersectInfo& info) const {
    return intersector->intersect(ray, info);
  }

  Vec3 getSkyRadiance(const Ray& ray) const { return sky->radiance(ray); }
};

}  // namespace LTRE

#endif