#ifndef _LTRE_SCENE_H
#define _LTRE_SCENE_H
#include <memory>

#include "LTRE/bsdf/lambert.hpp"
#include "LTRE/core/model.hpp"
#include "LTRE/intersector/intersector.hpp"

namespace LTRE {

class Scene {
 private:
  std::shared_ptr<Intersector<Primitive>> intersector;

  std::shared_ptr<BSDF> createBSDF(const Model::Material& material) {
    return std::make_shared<Lambert>(material.kd);
  }

 public:
  Scene(const std::shared_ptr<Intersector<Primitive>>& intersector)
      : intersector(intersector) {}

  void addPrimitive(const Primitive& primitive) {
    intersector->addPrimitive(primitive);
  }

  void addModel(const Model& model) {
    // for each meshes
    for (unsigned int i = 0; i < model.meshes.size(); ++i) {
      // create Primitive
      const std::shared_ptr<Mesh> shape = model.meshes[i];
      const auto bsdf = createBSDF(model.materials[i]);
      const Primitive prim = Primitive(shape, bsdf);

      // add Primitive to intersector
      intersector->addPrimitive(prim);
    }
  }

  void build() { intersector->build(); }

  bool intersect(const Ray& ray, IntersectInfo& info) const {
    return intersector->intersect(ray, info);
  }
};

}  // namespace LTRE

#endif