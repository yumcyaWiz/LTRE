#ifndef _LTRE_MESH_H
#define _LTRE_MESH_H
#include <memory>
#include <vector>

#include "LTRE/intersector/bvh.hpp"
#include "LTRE/intersector/linear-intersector.hpp"
#include "LTRE/shape/shape.hpp"

namespace LTRE {

class Mesh : public Shape {
 private:
  struct MeshTriangle {
    const Vec3* positions;        // pointer to vertex position array
    const unsigned int* indices;  // pointer to index array
    unsigned int faceID;          // which face this triangle represents

    const Vec3* normals;    // pointer to vertex normal array
    const Vec2* texcoords;  // pointer to texture coordinates
    const Vec3* tangents;   // pointer to tangent vector array
    const Vec3* dndus;      // pointer to dndu array
    const Vec3* dndvs;      // pointer to dndv array

    MeshTriangle()
        : positions(nullptr),
          indices(nullptr),
          faceID(0),
          normals(nullptr),
          texcoords(nullptr),
          tangents(nullptr),
          dndus(nullptr),
          dndvs(nullptr) {}

    MeshTriangle(const Vec3* positions, unsigned int* indices,
                 unsigned int faceID, const Vec3* normals = nullptr,
                 const Vec2* texcoords = nullptr,
                 const Vec3* tangents = nullptr, const Vec3* dndus = nullptr,
                 const Vec3* dndvs = nullptr)
        : positions(positions),
          indices(indices),
          faceID(faceID),
          normals(normals),
          texcoords(texcoords),
          tangents(tangents),
          dndus(dndus),
          dndvs(dndvs) {}

    // get vertex indices of this triangle
    std::tuple<unsigned int, unsigned int, unsigned int> getIndices() const {
      const unsigned int idx = 3 * faceID;
      return {indices[idx], indices[idx + 1], indices[idx + 2]};
    }

    // get vertex positions of this triangle
    std::tuple<Vec3, Vec3, Vec3> getPositions() const {
      const auto [i1, i2, i3] = getIndices();
      return {positions[i1], positions[i2], positions[i3]};
    }

    bool hasNormals() const { return normals != nullptr; }

    std::tuple<Vec3, Vec3, Vec3> getNormals() const {
      const auto [i1, i2, i3] = getIndices();
      return {normals[i1], normals[i2], normals[i3]};
    }

    bool hasTexcoords() const { return texcoords != nullptr; }

    std::tuple<Vec2, Vec2, Vec2> getTexcoords() const {
      const auto [i1, i2, i3] = getIndices();
      return {texcoords[i1], texcoords[i2], texcoords[i3]};
    }

    bool hasTangents() const { return tangents != nullptr; }

    std::tuple<Vec3, Vec3, Vec3> getTangents() const {
      const auto [i1, i2, i3] = getIndices();
      return {tangents[i1], tangents[i2], tangents[i3]};
    }

    AABB aabb() const {
      const auto [v1, v2, v3] = getPositions();

      constexpr float EPS = 1e-8f;
      Vec3 pMin, pMax;
      for (int i = 0; i < 3; ++i) {
        pMin[i] = std::min(std::min(v1[i], v2[i]), v3[i]);
        pMax[i] = std::max(std::max(v1[i], v2[i]), v3[i]);
      }
      return AABB(pMin - EPS, pMax + EPS);
    }

    bool intersect(const Ray& ray, IntersectInfo& info) const {
      const auto [v1, v2, v3] = getPositions();

      // https://www.tandfonline.com/doi/abs/10.1080/10867651.1997.10487468
      constexpr float EPS = 1e-8;
      const Vec3 e1 = v2 - v1;
      const Vec3 e2 = v3 - v1;

      const Vec3 pvec = cross(ray.direction, e2);
      const float det = dot(e1, pvec);

      if (det > -EPS && det < EPS) return false;
      const float invDet = 1.0f / det;

      const Vec3 tvec = ray.origin - v1;
      const float u = dot(tvec, pvec) * invDet;
      if (u < 0.0f || u > 1.0f) return false;

      const Vec3 qvec = cross(tvec, e1);
      const float v = dot(ray.direction, qvec) * invDet;
      if (v < 0.0f || u + v > 1.0f) return false;

      const float t = dot(e2, qvec) * invDet;
      if (t < ray.tmin || t > ray.tmax) return false;

      info.t = t;
      info.hitPos = ray(t);
      info.barycentric[0] = u;
      info.barycentric[1] = v;

      // calc normal
      const float w = 1.0f - u - v;
      if (hasNormals()) {
        // interpolated normal
        const auto [n1, n2, n3] = getNormals();
        info.hitNormal = w * n1 + u * n2 + v * n3;
      } else {
        // face normal
        info.hitNormal = normalize(cross(e1, e2));
      }

      // calc texcoords
      if (hasTexcoords()) {
        // interpolated texcoords
        const auto [texcoord1, texcoord2, texcoord3] = getTexcoords();
        info.uv = w * texcoord1 + u * texcoord2 + v * texcoord3;
      } else {
        // barycentric
        info.uv[0] = u;
        info.uv[1] = v;
      }

      return true;
    }
  };

  const std::vector<Vec3> positions;  // vertex position
  const std::vector<unsigned int> indices;
  const std::vector<Vec3> normals;    // vertex normal
  const std::vector<Vec2> texcoords;  // texture coordinates
  const std::vector<Vec3> tangents;   // tangent vector(dp/du)
  const std::vector<Vec3> dndus;      // differential of normal by texcoords
  const std::vector<Vec3> dndvs;      // differential of normal by texcoords

  LinearIntersector<MeshTriangle> intersector;

  void setupIntersector() {
    // populate intersector
    for (unsigned int i = 0; i < positions.size(); i += 3) {
      // make MeshTriangle
      MeshTriangle triangle;
      triangle.positions = positions.data();
      triangle.indices = indices.data();
      triangle.faceID = i / 3;
      if (normals.size() > 0) {
        triangle.normals = normals.data();
      }
      if (texcoords.size() > 0) {
        triangle.texcoords = texcoords.data();
      }
      if (tangents.size() > 0) {
        triangle.tangents = tangents.data();
      }
      if (dndus.size() > 0) {
        triangle.dndus = dndus.data();
      }
      if (dndvs.size() > 0) {
        triangle.dndvs = dndvs.data();
      }

      // add MeshTriangle to intersector
      intersector.addPrimitive(triangle);
    }

    // build intersector
    intersector.build();
  }

 public:
  Mesh(const std::vector<Vec3>& positions,
       const std::vector<unsigned int>& indices,
       const std::vector<Vec3>& normals, const std::vector<Vec2>& texcoords,
       const std::vector<Vec3>& tangents, const std::vector<Vec3>& dndus,
       const std::vector<Vec3>& dndvs)
      : positions(positions),
        indices(indices),
        normals(normals),
        texcoords(texcoords),
        tangents(tangents),
        dndus(dndus),
        dndvs(dndvs) {
    setupIntersector();
  }

  int nVertices() const { return positions.size(); }
  int nFaces() const { return positions.size() / 3; }

  bool intersect(const Ray& ray, IntersectInfo& info) const override {
    return intersector.intersect(ray, info);
  }

  AABB aabb() const override { return intersector.aabb(); }
};

}  // namespace LTRE

#endif