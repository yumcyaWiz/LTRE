#include "LTRE/shape/mesh.hpp"

namespace LTRE {

MeshTriangle::MeshTriangle()
    : positions(nullptr),
      indices(nullptr),
      faceID(0),
      normals(nullptr),
      texcoords(nullptr),
      tangents(nullptr),
      dndus(nullptr),
      dndvs(nullptr) {}

MeshTriangle::MeshTriangle(const Vec3* positions, unsigned int* indices,
                           unsigned int faceID, const Vec3* normals,
                           const Vec2* texcoords, const Vec3* tangents,
                           const Vec3* dndus, const Vec3* dndvs)
    : positions(positions),
      indices(indices),
      faceID(faceID),
      normals(normals),
      texcoords(texcoords),
      tangents(tangents),
      dndus(dndus),
      dndvs(dndvs) {}

std::tuple<unsigned int, unsigned int, unsigned int> MeshTriangle::getIndices()
    const {
  const unsigned int idx = 3 * faceID;
  return {indices[idx], indices[idx + 1], indices[idx + 2]};
}

std::tuple<Vec3, Vec3, Vec3> MeshTriangle::getPositions() const {
  const auto [i1, i2, i3] = getIndices();
  return {positions[i1], positions[i2], positions[i3]};
}

bool MeshTriangle::hasNormals() const { return normals != nullptr; }

std::tuple<Vec3, Vec3, Vec3> MeshTriangle::getNormals() const {
  const auto [i1, i2, i3] = getIndices();
  return {normals[i1], normals[i2], normals[i3]};
}

bool MeshTriangle::hasTexcoords() const { return texcoords != nullptr; }

std::tuple<Vec2, Vec2, Vec2> MeshTriangle::getTexcoords() const {
  const auto [i1, i2, i3] = getIndices();
  return {texcoords[i1], texcoords[i2], texcoords[i3]};
}

bool MeshTriangle::hasTangents() const { return tangents != nullptr; }

std::tuple<Vec3, Vec3, Vec3> MeshTriangle::getTangents() const {
  const auto [i1, i2, i3] = getIndices();
  return {tangents[i1], tangents[i2], tangents[i3]};
}

AABB MeshTriangle::aabb() const {
  const auto [v1, v2, v3] = getPositions();

  constexpr float EPS = 1e-8f;
  Vec3 pMin, pMax;
  for (int i = 0; i < 3; ++i) {
    pMin[i] = std::min(std::min(v1[i], v2[i]), v3[i]);
    pMax[i] = std::max(std::max(v1[i], v2[i]), v3[i]);
  }
  return AABB(pMin - EPS, pMax + EPS);
}

bool MeshTriangle::intersect(const Ray& ray, IntersectInfo& info) const {
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
  info.barycentric[0] = u;
  info.barycentric[1] = v;

  SurfaceInfo surfaceInfo;
  surfaceInfo.position = ray(t);

  // calc normal
  const float w = 1.0f - u - v;
  if (hasNormals()) {
    // interpolated normal
    const auto [n1, n2, n3] = getNormals();
    surfaceInfo.normal = w * n1 + u * n2 + v * n3;
  } else {
    // face normal
    surfaceInfo.normal = normalize(cross(e1, e2));
  }

  // calc texcoords
  if (hasTexcoords()) {
    // interpolated texcoords
    const auto [texcoord1, texcoord2, texcoord3] = getTexcoords();
    surfaceInfo.uv = w * texcoord1 + u * texcoord2 + v * texcoord3;
  } else {
    // barycentric
    surfaceInfo.uv[0] = u;
    surfaceInfo.uv[1] = v;
  }
  info.surfaceInfo = surfaceInfo;

  return true;
}

bool MeshTriangle::intersectP(const Ray& ray) const {
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

  return true;
}

void Mesh::setupIntersector() {
  // choose intersector
  if (nFaces() > 4) {
    intersector = std::make_shared<BVH<MeshTriangle, BVHSplitStrategy::SAH>>();
  } else {
    intersector = std::make_shared<BVH<MeshTriangle, BVHSplitStrategy::SAH>>();
  }

  // populate intersector
  for (unsigned int i = 0; i < nFaces(); i++) {
    // make MeshTriangle
    MeshTriangle triangle;
    triangle.positions = positions.data();
    triangle.indices = indices.data();
    triangle.faceID = i;
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
    intersector->addPrimitive(triangle);
  }

  // build intersector
  intersector->build();
}

Mesh::Mesh(const std::vector<Vec3>& positions,
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
  if (indices.size() % 3 != 0) {
    spdlog::error("[Mesh] untriangulated mesh detected.");
    std::exit(EXIT_FAILURE);
  }

  // compute surface area
  surfaceArea_ = 0;
  for (unsigned int f = 0; f < nFaces(); f += 3) {
    const int idx1 = indices[3 * f];
    const int idx2 = indices[3 * f + 1];
    const int idx3 = indices[3 * f + 2];
    const Vec3 p1 = positions[idx1];
    const Vec3 p2 = positions[idx2];
    const Vec3 p3 = positions[idx3];
    surfaceArea_ += 0.5f * length(cross(p2 - p1, p3 - p1));
  }

  setupIntersector();
}

unsigned int Mesh::nVertices() const { return indices.size(); }

unsigned int Mesh::nFaces() const { return indices.size() / 3; }

float Mesh::getSurfaceArea() const { return surfaceArea_; }

bool Mesh::intersect(const Ray& ray, IntersectInfo& info) const {
  return intersector->intersect(ray, info);
}

bool Mesh::intersectP(const Ray& ray) const {
  return intersector->intersectP(ray);
}

AABB Mesh::aabb() const { return intersector->aabb(); }

float Mesh::surfaceArea() const { return surfaceArea_; }

SurfaceInfo Mesh::samplePoint(Sampler& sampler, float& pdf) const {
  SurfaceInfo ret;

  // sample triangle
  unsigned int faceID = nFaces() * sampler.getNext1D();
  if (faceID == nFaces()) faceID--;

  // sample point on triangle
  const unsigned int idx1 = indices[3 * faceID];
  const unsigned int idx2 = indices[3 * faceID + 1];
  const unsigned int idx3 = indices[3 * faceID + 2];
  const Vec3 p1 = positions[idx1];
  const Vec3 p2 = positions[idx2];
  const Vec3 p3 = positions[idx3];

  // sample point on triangle
  const Vec2 uv = sampler.getNext2D();
  const float su0 = std::sqrt(uv[0]);
  const float u = 1.0f - su0;
  const float v = uv[1] * su0;
  ret.position = (1.0f - u - v) * p1 + u * p2 + v * p3;

  // compute normal
  if (normals.size() > 0) {
    const Vec3 n1 = normals[idx1];
    const Vec3 n2 = normals[idx2];
    const Vec3 n3 = normals[idx3];
    ret.normal = (1.0f - u - v) * n1 + u * n2 + v * n3;
  } else {
    ret.normal = normalize(cross(p2 - p1, p3 - p1));
  }

  // compute uv
  if (texcoords.size() > 0) {
    const Vec2 uv1 = texcoords[idx1];
    const Vec2 uv2 = texcoords[idx2];
    const Vec2 uv3 = texcoords[idx3];
    ret.uv = (1.0f - u - v) * uv1 + u * uv2 + v * uv3;
  } else {
    ret.uv = Vec2(u, v);
  }

  // compute pdf
  const float triSurfaceArea = 0.5f * length(cross(p2 - p1, p3 - p1));
  // NOTE: we can't use 1.0f / meshSurfaceArea, since sampling triangle is not
  // uniform across all triangles.
  pdf = 1.0f / (nFaces() * triSurfaceArea);

  return ret;
}

}  // namespace LTRE