#ifndef _LTRE_MODEL_H
#define _LTRE_MODEL_H
#include <optional>
#include <vector>

#include "LTRE/math/vec2.hpp"
#include "LTRE/math/vec3.hpp"

namespace LTRE {

class Model {
 private:
  struct Vertex {
    Vec3 position;   // vertex position
    Vec3 normal;     // vertex normal
    Vec2 texcoords;  // texture coordinates
    Vec3 tangent;    // tangent vector(dp/du)
    Vec3 dndu;       // differential of normal by texcoords
    Vec3 dndv;       // differential of normal by texcoords

    Vertex() {}
  };

  struct Material {
    Vec3 kd;  // diffuse color
    Vec3 ks;  // specular color
    Vec3 ka;  // ambient color
    Vec3 ke;  // emissive color
    float shininess;

    Material() {}
  };

 public:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  Material material;

  Model(const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices, const Material& material)
      : vertices(vertices), indices(indices), material(material) {}
};

}  // namespace LTRE

#endif