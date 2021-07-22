#ifndef _LTRE_MODEL_H
#define _LTRE_MODEL_H

#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

#include "assimp/scene.h"
//
#include "LTRE/core/material.hpp"
#include "LTRE/core/texture.hpp"
#include "LTRE/light/area-light.hpp"
#include "LTRE/math/vec2.hpp"
#include "LTRE/math/vec3.hpp"
#include "LTRE/shape/mesh.hpp"

namespace LTRE {

class Model {
 private:
  void loadModel(const std::filesystem::path& filepath);

  void processNode(const aiNode* node, const aiScene* scene,
                   const std::filesystem::path& parentPath);

  void processMesh(const aiMesh* mesh, const aiScene* scene,
                   const std::filesystem::path& parentPath);

  // load texture
  std::optional<std::size_t> loadTexture(
      const aiMaterial* material, const aiTextureType& type,
      const std::filesystem::path& parentPath);

  // check if we have load specified texture already
  std::optional<std::size_t> hasTexture(
      const std::filesystem::path& filepath) const;

 public:
  struct MaterialM {
    Vec3 kd;  // diffuse color
    Vec3 ks;  // specular color
    Vec3 ka;  // ambient color
    Vec3 ke;  // emissive color

    float shininess;
    float reflectivity;
    float ior;

    std::optional<unsigned int> diffuseMap;    // index of diffuse map texture
    std::optional<unsigned int> specularMap;   // index of specular map texture
    std::optional<unsigned int> ambientMap;    // index of ambient map texture
    std::optional<unsigned int> emissiveMap;   // index of emissive map texture
    std::optional<unsigned int> heightMap;     // index of height map texture
    std::optional<unsigned int> normalMap;     // index of normal map texture
    std::optional<unsigned int> shininessMap;  // index of shininess map texture
    std::optional<unsigned int>
        displacementMap;                   // index of displacement map texture
    std::optional<unsigned int> lightMap;  // index of light map textur

    MaterialM() : shininess(0), reflectivity(0), ior(0) {}
  };

  std::vector<std::shared_ptr<Mesh>> meshes;
  std::vector<MaterialM> materials;
  std::vector<std::shared_ptr<ImageTexture>> textures;

  Model();
  Model(const std::filesystem::path& filepath);

  std::shared_ptr<Material> createMaterial(unsigned int idx) const;
  std::shared_ptr<AreaLight> createAreaLight(unsigned int idx) const;
};

}  // namespace LTRE

#endif