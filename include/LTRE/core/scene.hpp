#ifndef _LTRE_SCENE_H
#define _LTRE_SCENE_H
#include <memory>

#include "LTRE/core/model.hpp"
#include "LTRE/intersector/intersector.hpp"

namespace LTRE {

class Scene {
 private:
  const std::shared_ptr<Intersector<Primitive>> intersector;
  const std::shared_ptr<Light> sky;

  std::vector<std::shared_ptr<Light>> lights;

 public:
  Scene();
  Scene(const std::shared_ptr<Intersector<Primitive>>& intersector,
        const std::shared_ptr<Light>& sky);

  void addPrimitive(const Primitive& primitive);
  void addModel(const Model& model);

  void build();

  bool intersect(const Ray& ray, IntersectInfo& info) const;
  bool intersectP(const Ray& ray) const;

  Vec3 getSkyRadiance(const Ray& ray) const;

  std::shared_ptr<Light> sampleLight(Sampler& sampler, float& pdf) const;
};

}  // namespace LTRE

#endif