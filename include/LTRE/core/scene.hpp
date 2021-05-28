#ifndef _LTRE_SCENE_H
#define _LTRE_SCENE_H
#include <memory>

#include "LTRE/intersector/intersector.hpp"

namespace LTRE {

class Scene {
 private:
  std::shared_ptr<Intersector> intersector;

 public:
  Scene(const std::shared_ptr<Intersector>& intersector)
      : intersector(intersector) {}

  void addPrimitive(const Primitive& primitive) {
    intersector->addPrimitive(primitive);
  }

  bool intersect(const Ray& ray, IntersectInfo& info) const {
    return intersector->intersect(ray, info);
  }
};

}  // namespace LTRE

#endif