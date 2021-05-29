#ifndef _LTRE_POLYGON_H
#define _LTRE_POLYGON_H
#include <array>
#include <cassert>
#include <iostream>

#include "LTRE/math/vec3.hpp"

namespace LTRE {

struct Polygon {
  unsigned int nVertices;
  float* vertices;
  unsigned int* indices;
  float* normals;
  float* uvs;
  int* geomIDs;

  Polygon(unsigned int nVertices, float* vertices, unsigned int* indices,
          float* normals = nullptr, float* uvs = nullptr,
          int* geomIDs = nullptr)
      : nVertices(nVertices),
        vertices(vertices),
        indices(indices),
        normals(normals),
        uvs(uvs),
        geomIDs(geomIDs) {}

  Vec3 getVertex(unsigned int vertexIdx) const {
    assert(vertexIdx <= nVertices);
    return Vec3(vertices[3 * vertexIdx], vertices[3 * vertexIdx + 1],
                vertices[3 * vertexIdx + 2]);
  }

  std::array<unsigned int, 3> getIndices(unsigned int faceIdx) const {
    assert(faceIdx <= nFaces());
    return {indices[3 * faceIdx + 0], indices[3 * faceIdx + 1],
            indices[3 * faceIdx + 2]};
  }

  Vec3 getNormal(unsigned int vertexIdx) const {
    assert(vertexIdx <= nVertices);
    return Vec3(normals[3 * vertexIdx], normals[3 * vertexIdx + 1],
                normals[3 * vertexIdx + 2]);
  }

  std::pair<float, float> getUV(unsigned int vertexIdx) const {
    assert(vertexIdx <= nVertices);
    return {uvs[2 * vertexIdx], uvs[2 * vertexIdx + 1]};
  }

  bool hasNormals() const { return normals != nullptr; }
  bool hasUVs() const { return uvs != nullptr; }

  unsigned int nFaces() const { return nVertices / 3; };
};

}  // namespace LTRE

#endif