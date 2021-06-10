#ifndef _LTRE_MODEL_H
#define _LTRE_MODEL_H
#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "spdlog/spdlog.h"
//
#include "LTRE/core/texture.hpp"
#include "LTRE/light/area-light.hpp"
#include "LTRE/math/vec2.hpp"
#include "LTRE/math/vec3.hpp"
#include "LTRE/shape/mesh.hpp"

namespace LTRE {

class Model {
 private:
  void loadModel(const std::filesystem::path& filepath) {
    // load model with assimp
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        filepath.generic_string().c_str(),
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
      spdlog::error(importer.GetErrorString());
      return;
    }

    // process scene graph
    const std::filesystem::path ps(filepath);
    processNode(scene->mRootNode, scene, ps.parent_path());

    // show info
    spdlog::info("[Model] " + filepath.string() + " loaded.");
    spdlog::info("[Model] number of meshes: " + std::to_string(meshes.size()));

    std::size_t nVertices = 0;
    std::size_t nFaces = 0;
    for (std::size_t i = 0; i < meshes.size(); ++i) {
      nVertices += meshes[i]->nVertices();
      nFaces += meshes[i]->nFaces();
    }
    spdlog::info("[Model] number of vertices: " + std::to_string(nVertices));
    spdlog::info("[Model] number of faces: " + std::to_string(nFaces));
    spdlog::info("[Model] number of textures: " +
                 std::to_string(textures.size()));
  }

  void processNode(const aiNode* node, const aiScene* scene,
                   const std::filesystem::path& parentPath) {
    // process all the node's meshes
    for (std::size_t i = 0; i < node->mNumMeshes; ++i) {
      const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      processMesh(mesh, scene, parentPath);
    }

    for (std::size_t i = 0; i < node->mNumChildren; ++i) {
      processNode(node->mChildren[i], scene, parentPath);
    }
  }

  void processMesh(const aiMesh* mesh, const aiScene* scene,
                   const std::filesystem::path& parentPath) {
    std::vector<Vec3> positions;
    std::vector<unsigned int> indices;
    std::vector<Vec3> normals;
    std::vector<Vec2> texcoords;
    std::vector<Vec3> tangents;
    std::vector<Vec3> dndus;
    std::vector<Vec3> dndvs;
    Material material;

    spdlog::info("[Mesh] Processing " + std::string(mesh->mName.C_Str()));

    // vertices
    for (std::size_t i = 0; i < mesh->mNumVertices; ++i) {
      // position
      positions.emplace_back(mesh->mVertices[i].x, mesh->mVertices[i].y,
                             mesh->mVertices[i].z);

      // normal
      if (mesh->mNormals) {
        normals.emplace_back(mesh->mNormals[i].x, mesh->mNormals[i].y,
                             mesh->mNormals[i].z);
      }

      // texcoords
      if (mesh->mTextureCoords[0]) {
        const float u = mesh->mTextureCoords[0][i].x;
        const float v = mesh->mTextureCoords[0][i].y;
        texcoords.emplace_back(u, v);
      }

      // tangent
      if (mesh->mTangents) {
        tangents.emplace_back(mesh->mTangents[i].x, mesh->mTangents[i].y,
                              mesh->mTangents[i].z);
      }
    }

    // indices
    for (std::size_t i = 0; i < mesh->mNumFaces; ++i) {
      const aiFace& face = mesh->mFaces[i];
      for (std::size_t j = 0; j < face.mNumIndices; ++j) {
        indices.push_back(face.mIndices[j]);
      }
    }

    spdlog::info("[Mesh] number of vertices " + std::to_string(indices.size()));
    spdlog::info("[Mesh] number of faces " +
                 std::to_string(indices.size() / 3));

    // add mesh
    const auto mesh_ptr = std::make_shared<Mesh>(
        positions, indices, normals, texcoords, tangents, dndus, dndvs);
    meshes.push_back(mesh_ptr);

    // materials
    if (scene->mMaterials[mesh->mMaterialIndex]) {
      const aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

      if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        spdlog::info("[Mesh] Diffuse Map Detected");
      }
      if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
        spdlog::info("[Mesh] Specular Map Detected");
      }
      if (mat->GetTextureCount(aiTextureType_AMBIENT) > 0) {
        spdlog::info("[Mesh] Ambient Map Detected");
      }
      if (mat->GetTextureCount(aiTextureType_EMISSIVE) > 0) {
        spdlog::info("[Mesh] Emissive Map Detected");
      }
      if (mat->GetTextureCount(aiTextureType_EMISSIVE) > 0) {
        spdlog::info("[Mesh] Emissive Map Detected");
      }
      if (mat->GetTextureCount(aiTextureType_HEIGHT) > 0) {
        spdlog::info("[Mesh] Height Map Detected");
      }
      if (mat->GetTextureCount(aiTextureType_NORMALS) > 0) {
        spdlog::info("[Mesh] Normal Map Detected");
      }
      if (mat->GetTextureCount(aiTextureType_SHININESS) > 0) {
        spdlog::info("[Mesh] Shininess Map Detected");
      }
      if (mat->GetTextureCount(aiTextureType_DISPLACEMENT) > 0) {
        spdlog::info("[Mesh] Displacement Map Detected");
      }
      if (mat->GetTextureCount(aiTextureType_LIGHTMAP) > 0) {
        spdlog::info("[Mesh] Light Map Detected");
      }

      // kd
      aiColor3D color;
      mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
      // convert sRGB to linear
      material.kd = pow(Vec3(color.r, color.g, color.b), 2.2f);

      // ks
      mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
      material.ks = Vec3(color.r, color.g, color.b);

      // ka
      mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
      material.ka = Vec3(color.r, color.g, color.b);

      // ke
      mat->Get(AI_MATKEY_COLOR_EMISSIVE, color);
      material.ke = Vec3(color.r, color.g, color.b);

      // shininess
      mat->Get(AI_MATKEY_SHININESS, material.shininess);

      // reflectivity
      mat->Get(AI_MATKEY_REFLECTIVITY, material.reflectivity);

      // ior
      mat->Get(AI_MATKEY_REFRACTI, material.ior);

      // show material info
      spdlog::info("[Model] kd: " + material.kd.toString());
      spdlog::info("[Model] ks: " + material.ks.toString());
      spdlog::info("[Model] ka: " + material.ka.toString());
      spdlog::info("[Model] ke: " + material.ke.toString());
      spdlog::info("[Model] shininess: " + std::to_string(material.shininess));
      spdlog::info("[Model] reflectivity: " +
                   std::to_string(material.reflectivity));
      spdlog::info("[Model] ior: " + std::to_string(material.ior));

      // load textures
      // diffuse map
      material.diffuseMap = loadTexture(mat, aiTextureType_DIFFUSE, parentPath);

      // specular map
      material.specularMap =
          loadTexture(mat, aiTextureType_SPECULAR, parentPath);

      // ambient map
      material.ambientMap = loadTexture(mat, aiTextureType_AMBIENT, parentPath);

      // emissive map
      material.emissiveMap =
          loadTexture(mat, aiTextureType_EMISSIVE, parentPath);

      // height map
      material.heightMap = loadTexture(mat, aiTextureType_HEIGHT, parentPath);

      // normal map
      material.normalMap = loadTexture(mat, aiTextureType_NORMALS, parentPath);

      // shininess map
      material.shininessMap =
          loadTexture(mat, aiTextureType_SHININESS, parentPath);

      // displacement map
      material.displacementMap =
          loadTexture(mat, aiTextureType_DISPLACEMENT, parentPath);

      // light map
      material.lightMap = loadTexture(mat, aiTextureType_LIGHTMAP, parentPath);
    }

    // add material
    materials.push_back(material);
  }

  // load texture
  std::optional<std::size_t> loadTexture(
      const aiMaterial* material, const aiTextureType& type,
      const std::filesystem::path& parentPath) {
    // if there is no texture
    if (material->GetTextureCount(type) == 0) {
      return std::nullopt;
    }

    // get texture filepath
    aiString str;
    material->GetTexture(type, 0, &str);
    const std::filesystem::path texturePath = (parentPath / str.C_Str());

    // load texture if we don't have it
    const auto index = hasTexture(texturePath);
    if (!index) {
      const auto texture = std::make_shared<ImageTexture>(texturePath);
      textures.push_back(texture);
      return textures.size() - 1;
    } else {
      return index.value();
    }
  }

  // check if we have load specified texture already
  std::optional<std::size_t> hasTexture(
      const std::filesystem::path& filepath) const {
    for (std::size_t i = 0; i < textures.size(); ++i) {
      if (textures[i]->getFilepath() == filepath) {
        return i;
      }
    }
    return std::nullopt;
  }

 public:
  struct Material {
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

    Material() : shininess(0), reflectivity(0), ior(0) {}
  };

  std::vector<std::shared_ptr<Mesh>> meshes;
  std::vector<Material> materials;
  std::vector<std::shared_ptr<ImageTexture>> textures;

  Model() {}
  Model(const std::filesystem::path& filepath) { loadModel(filepath); }

  std::shared_ptr<BSDF> createBSDF(unsigned int idx) const {
    const Material& material = materials[idx];

    // diffuse
    std::shared_ptr<Texture<Vec3>> rho;
    if (material.diffuseMap) {
      rho = textures[material.diffuseMap.value()];
    } else {
      rho = std::make_shared<UniformTexture<Vec3>>(material.kd);
    }

    return std::make_shared<OrenNayer>(rho, 0.34);
  }

  std::shared_ptr<AreaLight> createAreaLight(unsigned int idx) const {
    const Material& material = materials[idx];

    bool hasLight = false;
    std::shared_ptr<Texture<Vec3>> le;
    if (material.emissiveMap) {
      le = textures[material.emissiveMap.value()];
      hasLight = true;
    } else if (material.ke) {
      le = std::make_shared<UniformTexture<Vec3>>(material.ke);
      hasLight = true;
    }

    if (hasLight) {
      return std::make_shared<AreaLight>(le, meshes[idx]);
    } else {
      return nullptr;
    }
  }
};

}  // namespace LTRE

#endif