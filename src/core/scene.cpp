#include "LTRE/core/scene.hpp"

namespace LTRE {

Scene::Scene() {}

Scene::Scene(const std::shared_ptr<Intersector<Primitive>>& intersector,
             const std::shared_ptr<Light>& sky)
    : intersector(intersector), sky(sky) {}

void Scene::addPrimitive(const Primitive& primitive) {
  intersector->addPrimitive(primitive);
}

void Scene::addModel(const Model& model) {
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

void Scene::build() {
  // build intersector
  intersector->build();
  const AABB sceneAABB = intersector->aabb();
  spdlog::info("[Scene] scene bounds: ({0}, {1}, {2}), ({3}, {4}, {5})",
               sceneAABB.bounds[0][0], sceneAABB.bounds[0][1],
               sceneAABB.bounds[0][2], sceneAABB.bounds[1][0],
               sceneAABB.bounds[1][1], sceneAABB.bounds[1][2]);

  // initialize lights
  // NOTE: only add lights which has power greater than 0
  for (const auto& prim : intersector->getPrimitivesRef()) {
    if (prim.hasArealight()) {
      const auto light = prim.getAreaLightPtr();
      if (light->power() > Vec3(0)) {
        lights.push_back(light);
      }
    }
  }
  // add sky to lights
  if (sky->power() > Vec3(0)) {
    lights.push_back(sky);
  }
  spdlog::info("[Scene] number of lights: {}", lights.size());
}

bool Scene::intersect(const Ray& ray, IntersectInfo& info) const {
  return intersector->intersect(ray, info);
}

bool Scene::intersectP(const Ray& ray) const {
  return intersector->intersectP(ray);
}

Vec3 Scene::getSkyRadiance(const Ray& ray) const {
  // TODO: more nice way?
  SurfaceInfo dummy;
  return sky->Le(ray.direction, dummy);
}

std::shared_ptr<Light> Scene::sampleLight(Sampler& sampler, float& pdf) const {
  unsigned int lightIdx = lights.size() * sampler.getNext1D();
  if (lightIdx == lights.size()) lightIdx--;
  pdf = 1.0f / lights.size();
  return lights[lightIdx];
}

}  // namespace LTRE