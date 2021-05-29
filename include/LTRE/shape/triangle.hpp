#ifndef _LTRE_TRIANGLE_H
#define _LTRE_TRIANGLE_H
#include "LTRE/core/aabb.hpp"
#include "LTRE/shape/shape.hpp"

namespace LTRE {

class Triangle : public Shape {
 private:
  unsigned int nVertices;  // number of vertices
  float* vertices;         // vertex array
  unsigned int* indices;   // index array
  unsigned int faceID;     // specify which face this triangle represents
  float* normals;          // normal array(optional)
  float* uvs;              // uv array(optional)

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

 public:
  Triangle(unsigned int nVertices, float* vertices, unsigned int* indices,
           unsigned int faceID, float* normals = nullptr, float* uvs = nullptr)
      : nVertices(nVertices),
        vertices(vertices),
        indices(indices),
        faceID(faceID),
        normals(normals),
        uvs(uvs) {}

  AABB aabb() const {
    const auto indices = getIndices(faceID);
    const Vec3 v1 = getVertex(indices[0]);
    const Vec3 v2 = getVertex(indices[1]);
    const Vec3 v3 = getVertex(indices[2]);

    Vec3 pMin, pMax;
    for (int i = 0; i < 3; ++i) {
      pMin[i] = std::min(std::min(v1[i], v2[i]), v3[i]);
      pMax[i] = std::max(std::max(v1[i], v2[i]), v3[i]);
    }

    return AABB(pMin, pMax);
  }

  bool intersect(const Ray& ray, IntersectInfo& info) const {
    const auto indices = getIndices(faceID);
    const Vec3 v1 = getVertex(indices[0]);
    const Vec3 v2 = getVertex(indices[1]);
    const Vec3 v3 = getVertex(indices[2]);

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
      const Vec3 n1 = getNormal(indices[0]);
      const Vec3 n2 = getNormal(indices[1]);
      const Vec3 n3 = getNormal(indices[2]);
      info.hitPos = w * n1 + u * n2 + v * n3;
    } else {
      // face normal
      info.hitNormal = normalize(cross(e1, e2));
    }

    // calc uv
    if (hasUVs()) {
      // interpolated uv
      const auto uv1 = getUV(indices[0]);
      const auto uv2 = getUV(indices[1]);
      const auto uv3 = getUV(indices[2]);
      info.uv[0] = w * uv1.first + u * uv2.first + v * uv3.first;
      info.uv[1] = w * uv1.second + u * uv2.second + v * uv3.second;
    } else {
      // barycentric uv
      info.uv[0] = u;
      info.uv[1] = v;
    }

    return true;
  }
};

}  // namespace LTRE

#endif