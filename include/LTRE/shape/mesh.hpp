#ifndef _LTRE_MESH_H
#define _LTRE_MESH_H
#include <memory>
#include <vector>

#include "spdlog/spdlog.h"
//
#include "LTRE/intersector/bvh.hpp"
#include "LTRE/intersector/qbvh.hpp"
#include "LTRE/shape/shape.hpp"

namespace LTRE {

struct MeshTriangle {
  const Vec3* positions;        // pointer to vertex position array
  const unsigned int* indices;  // pointer to index array
  unsigned int faceID;          // which face this triangle represents

  const Vec3* normals;    // pointer to vertex normal array
  const Vec2* texcoords;  // pointer to texture coordinates
  const Vec3* tangents;   // pointer to tangent vector array
  const Vec3* dndus;      // pointer to dndu array
  const Vec3* dndvs;      // pointer to dndv array

  MeshTriangle();

  MeshTriangle(const Vec3* positions, unsigned int* indices,
               unsigned int faceID, const Vec3* normals = nullptr,
               const Vec2* texcoords = nullptr, const Vec3* tangents = nullptr,
               const Vec3* dndus = nullptr, const Vec3* dndvs = nullptr);

  // get vertex indices of this triangle
  std::tuple<unsigned int, unsigned int, unsigned int> getIndices() const;

  // get vertex positions of this triangle
  std::tuple<Vec3, Vec3, Vec3> getPositions() const;

  bool hasNormals() const;
  std::tuple<Vec3, Vec3, Vec3> getNormals() const;

  bool hasTexcoords() const;
  std::tuple<Vec2, Vec2, Vec2> getTexcoords() const;

  bool hasTangents() const;
  std::tuple<Vec3, Vec3, Vec3> getTangents() const;

  AABB aabb() const;

  bool intersect(const Ray& ray, IntersectInfo& info) const;
  bool intersectP(const Ray& ray) const;
};

class Mesh : public Shape {
 private:
  const std::vector<Vec3> positions;  // vertex position
  const std::vector<unsigned int> indices;
  const std::vector<Vec3> normals;    // vertex normal
  const std::vector<Vec2> texcoords;  // texture coordinates
  const std::vector<Vec3> tangents;   // tangent vector(dp/du)
  const std::vector<Vec3> dndus;      // differential of normal by texcoords
  const std::vector<Vec3> dndvs;      // differential of normal by texcoords
  float surfaceArea_;

  std::shared_ptr<Intersector<MeshTriangle>> intersector;

  void setupIntersector();

 public:
  Mesh(const std::vector<Vec3>& positions,
       const std::vector<unsigned int>& indices,
       const std::vector<Vec3>& normals, const std::vector<Vec2>& texcoords,
       const std::vector<Vec3>& tangents, const std::vector<Vec3>& dndus,
       const std::vector<Vec3>& dndvs);

  unsigned int nVertices() const;
  unsigned int nFaces() const;
  float getSurfaceArea() const;

  bool intersect(const Ray& ray, IntersectInfo& info) const override;
  bool intersectP(const Ray& ray) const override;
  AABB aabb() const override;
  float surfaceArea() const override;
  SurfaceInfo samplePoint(Sampler& sampler, float& pdf) const override;
};

}  // namespace LTRE

#endif