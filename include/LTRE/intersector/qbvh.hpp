#ifndef _LTRE_QBVH_H
#define _LTRE_QBVH_H
#include <immintrin.h>

#include "LTRE/intersector/bvh.hpp"
#include "LTRE/intersector/intersector.hpp"

namespace LTRE {

// TODO: fix intersection bug, problem maybe relates to construction of tree or
// intersection with AABB
// TODO: implement CENTER splitting, SAH splitting case, we need to handle 0
// primitives case
template <Intersectable T, BVHSplitStrategy strategy>
class QBVH : public Intersector<T> {
 private:
  struct alignas(128) BVHNode {
    float bounds[4 * 2 * 3];
    int child[4];
    int axisTop;
    int axisLeft;
    int axisRight;
  };

  std::vector<BVHNode> nodes;
  BVH<T, strategy>::BVHStatistics stats;

  static int encodeLeaf(int nPrims, int primStart) {
    if (nPrims > 0xf) {
      spdlog::error("[QBVH] nPrims out of bounds");
      std::exit(EXIT_FAILURE);
    }

    int enc = 0;
    enc |= (1 << 31);
    enc |= ((nPrims & 0xf) << 27);
    enc |= (primStart & 0x07ffffff);
    return enc;
  }

  static void decodeLeaf(int child, int& nPrims, int& primitivesOffset) {
    nPrims = (child & 0x78000000) >> 27;
    primitivesOffset = (child & 0x07ffffff);
  }

  static bool isLeaf(int child) { return ((child & 0x80000000) >> 31) == 1; }

  // build bvh node recursively
  // NOTE: we need at least 4 primitives
  void buildBVHNode(int primStart, int primEnd) {
    // compute top AABB
    const AABB topAABB =
        BVH<T, strategy>::computeAABB(this->primitives, primStart, primEnd);

    // split top AABB
    int splitAxisTop, splitIdxTop;
    bool makeLeafTop;
    BVH<T, strategy>::splitAABB(topAABB, this->primitives, primStart, primEnd,
                                splitAxisTop, splitIdxTop, makeLeafTop, true);

    // compute left AABB
    const AABB leftAABB =
        BVH<T, strategy>::computeAABB(this->primitives, primStart, splitIdxTop);

    // split left AABB
    int splitAxisLeft, splitIdxLeft;
    bool makeLeafLeft;
    BVH<T, strategy>::splitAABB(leftAABB, this->primitives, primStart,
                                splitIdxTop, splitAxisLeft, splitIdxLeft,
                                makeLeafLeft, true);

    // compute right AABB
    const AABB rightAABB =
        BVH<T, strategy>::computeAABB(this->primitives, splitIdxTop, primEnd);

    // split right AABB
    int splitAxisRight, splitIdxRight;
    bool makeLeafRight;
    BVH<T, strategy>::splitAABB(rightAABB, this->primitives, splitIdxTop,
                                primEnd, splitAxisRight, splitIdxRight,
                                makeLeafRight, true);

    assert(primStart != splitIdxTop);
    assert(splitIdxLeft != splitIdxTop);
    assert(splitIdxRight != splitIdxTop);
    assert(splitIdxRight != primEnd);

    // compute children AABB
    const AABB bbox0 = BVH<T, strategy>::computeAABB(this->primitives,
                                                     primStart, splitIdxLeft);
    const AABB bbox1 = BVH<T, strategy>::computeAABB(this->primitives,
                                                     splitIdxLeft, splitIdxTop);
    const AABB bbox2 = BVH<T, strategy>::computeAABB(
        this->primitives, splitIdxTop, splitIdxRight);
    const AABB bbox3 =
        BVH<T, strategy>::computeAABB(this->primitives, splitIdxRight, primEnd);

    // populate node info
    const AABB childboxes[4] = {bbox0, bbox1, bbox2, bbox3};
    BVHNode node;
    for (int i = 0; i < 4; ++i) {
      node.bounds[i] = childboxes[i].bounds[0][0];
      node.bounds[i + 4] = childboxes[i].bounds[0][1];
      node.bounds[i + 8] = childboxes[i].bounds[0][2];
      node.bounds[i + 12] = childboxes[i].bounds[1][0];
      node.bounds[i + 16] = childboxes[i].bounds[1][1];
      node.bounds[i + 20] = childboxes[i].bounds[1][2];
    }
    node.axisTop = splitAxisTop;
    node.axisLeft = splitAxisLeft;
    node.axisRight = splitAxisRight;

    const int nPrimsChild[4] = {
        splitIdxLeft - primStart, splitIdxTop - splitIdxLeft,
        splitIdxRight - splitIdxTop, primEnd - splitIdxRight};
    const int primStartChild[4] = {primStart, splitIdxLeft, splitIdxTop,
                                   splitIdxRight};

    // add parent node without child info
    // NOTE: populate child info later
    const int parentOffset = nodes.size();
    nodes.push_back(node);
    stats.nInternalNodes++;

    constexpr int leafThreshold = 15;
    if (nPrimsChild[0] <= leafThreshold) {
      // make leaf node
      nodes[parentOffset].child[0] =
          encodeLeaf(nPrimsChild[0], primStartChild[0]);
      stats.nLeafNodes++;
    } else {
      // set index of child0
      const int child0Offset = nodes.size();
      nodes[parentOffset].child[0] = child0Offset;

      // build child0 subtree
      buildBVHNode(primStart, splitIdxLeft);
    }

    // build child1 subtree
    if (nPrimsChild[1] <= leafThreshold) {
      // make leaf node
      nodes[parentOffset].child[1] =
          encodeLeaf(nPrimsChild[1], primStartChild[1]);
      stats.nLeafNodes++;
    } else {
      // set index of child1
      const int child1Offset = nodes.size();
      nodes[parentOffset].child[1] = child1Offset;

      // build child1 subtree
      buildBVHNode(splitIdxLeft, splitIdxTop);
    }

    // build child2 subtree
    if (nPrimsChild[2] <= leafThreshold) {
      // make leaf node
      nodes[parentOffset].child[2] =
          encodeLeaf(nPrimsChild[2], primStartChild[2]);
      stats.nLeafNodes++;
    } else {
      // set index of child2
      const int child2Offset = nodes.size();
      nodes[parentOffset].child[2] = child2Offset;

      // build child2 subtree
      buildBVHNode(splitIdxTop, splitIdxRight);
    }

    // build child3 subtree
    if (nPrimsChild[3] <= leafThreshold) {
      // make leaf node
      nodes[parentOffset].child[3] =
          encodeLeaf(nPrimsChild[3], primStartChild[3]);
      stats.nLeafNodes++;
    } else {
      // set index of child3
      const int child3Offset = nodes.size();
      nodes[parentOffset].child[3] = child3Offset;

      // build child3 subtree
      buildBVHNode(splitIdxRight, primEnd);
    }
  }

 public:
  // intersect 4 aabb with SIMD
  static int intersectAABB(const __m128 orig[3], const __m128 dirInv[3],
                           const int dirInvSign[3], const __m128 raytmin,
                           const __m128 raytmax, const __m128 bounds[2][3]) {
    // SIMD version of https://dl.acm.org/doi/abs/10.1145/1198555.1198748
    __m128 tmin =
        _mm_mul_ps(_mm_sub_ps(bounds[dirInvSign[0]][0], orig[0]), dirInv[0]);
    __m128 tmax = _mm_mul_ps(_mm_sub_ps(bounds[1 - dirInvSign[0]][0], orig[0]),
                             dirInv[0]);

    tmin = _mm_max_ps(
        tmin,
        _mm_mul_ps(_mm_sub_ps(bounds[dirInvSign[1]][1], orig[1]), dirInv[1]));
    tmax = _mm_min_ps(
        tmax, _mm_mul_ps(_mm_sub_ps(bounds[1 - dirInvSign[1]][1], orig[1]),
                         dirInv[1]));

    tmin = _mm_max_ps(
        tmin,
        _mm_mul_ps(_mm_sub_ps(bounds[dirInvSign[2]][2], orig[2]), dirInv[2]));
    tmax = _mm_min_ps(
        tmax, _mm_mul_ps(_mm_sub_ps(bounds[1 - dirInvSign[2]][2], orig[2]),
                         dirInv[2]));

    const __m128 comp1 = _mm_cmp_ps(tmax, tmin, _CMP_GT_OQ);
    const __m128 comp2 = _mm_and_ps(_mm_cmp_ps(tmin, raytmax, _CMP_LT_OQ),
                                    _mm_cmp_ps(tmax, raytmin, _CMP_GT_OQ));
    return _mm_movemask_ps(_mm_and_ps(comp1, comp2));
  }

 private:
  // traverse QBVH recursively
  bool intersectNode(int nodeIdx, const Ray& ray, const __m128 orig[3],
                     const __m128 dirInv[3], const int dirInvSign[3],
                     IntersectInfo& info) const {
    bool hit = false;
    const BVHNode& node = nodes[nodeIdx];

    // prepare SIMD data
    const __m128 raytmin = _mm_set_ps1(ray.tmin);
    const __m128 raytmax = _mm_set_ps1(ray.tmax);
    __m128 bounds[2][3];
    bounds[0][0] = _mm_load_ps(&node.bounds[0]);
    bounds[0][1] = _mm_load_ps(&node.bounds[4]);
    bounds[0][2] = _mm_load_ps(&node.bounds[8]);
    bounds[1][0] = _mm_load_ps(&node.bounds[12]);
    bounds[1][1] = _mm_load_ps(&node.bounds[16]);
    bounds[1][2] = _mm_load_ps(&node.bounds[20]);

    // intersect AABB
    const int hitMask =
        intersectAABB(orig, dirInv, dirInvSign, raytmin, raytmax, bounds);
    const int hitChild[4] = {hitMask & 0b1, hitMask & 0b10, hitMask & 0b100,
                             hitMask & 0b1000};

    for (int i = 0; i < 4; ++i) {
      if (hitChild[i] > 0) {
        const int child = node.child[i];
        // leaf node
        if (isLeaf(child)) {
          // unpack leaf data
          int nPrims, primitivesOffset;
          decodeLeaf(child, nPrims, primitivesOffset);
          // test intersection with all primitives in this node
          for (int j = primitivesOffset; j < primitivesOffset + nPrims; ++j) {
            if (this->primitives[j].intersect(ray, info)) {
              ray.tmax = info.t;
              hit = true;
            }
          }
        }
        // internal node
        else {
          // test intersection with child node
          hit |= intersectNode(child, ray, orig, dirInv, dirInvSign, info);
        }
      }
    }

    return hit;
  }

 public:
  QBVH() {}
  QBVH(const std::vector<T>& primitives) : Intersector<T>(primitives) {}

  bool build() override {
    // start building bvh from root node
    buildBVHNode(0, this->primitives.size());
    stats.nNodes = stats.nInternalNodes + stats.nLeafNodes;
    spdlog::info("[QBVH] nPrimitives: " +
                 std::to_string(this->primitives.size()));
    spdlog::info("[QBVH] nNodes: " + std::to_string(stats.nNodes));
    spdlog::info("[QBVH] nInternalNodes: " +
                 std::to_string(stats.nInternalNodes));
    spdlog::info("[QBVH] nLeafNodes: " + std::to_string(stats.nLeafNodes));

    return true;
  }

  bool intersect(const Ray& ray, IntersectInfo& info) const override {
    // precompute ray's inversed direction, sign of direction
    const Vec3 dirInv = 1.0f / ray.direction;
    int dirInvSign[3];
    for (int i = 0; i < 3; ++i) {
      dirInvSign[i] = dirInv[i] > 0 ? 0 : 1;
    }

    // prepare simd data
    const __m128 orig[3] = {_mm_set_ps1(ray.origin[0]),
                            _mm_set_ps1(ray.origin[1]),
                            _mm_set_ps1(ray.origin[2])};
    const __m128 _dirInv[3] = {_mm_set_ps1(dirInv[0]), _mm_set_ps1(dirInv[1]),
                               _mm_set_ps1(dirInv[2])};

    // traverse from root node
    return intersectNode(0, ray, orig, _dirInv, dirInvSign, info);
  }

  AABB aabb() const override {
    if (nodes.size() > 0) {
      const AABB bbox0 = AABB(
          Vec3(nodes[0].bounds[0], nodes[0].bounds[4], nodes[0].bounds[8]),
          Vec3(nodes[0].bounds[12], nodes[0].bounds[16], nodes[0].bounds[20]));
      const AABB bbox1 =
          AABB(Vec3(nodes[0].bounds[0 + 1], nodes[0].bounds[4 + 1],
                    nodes[0].bounds[8 + 1]),
               Vec3(nodes[0].bounds[12 + 1], nodes[0].bounds[16 + 1],
                    nodes[0].bounds[20 + 1]));
      const AABB bbox2 =
          AABB(Vec3(nodes[0].bounds[0 + 2], nodes[0].bounds[4 + 2],
                    nodes[0].bounds[8 + 2]),
               Vec3(nodes[0].bounds[12 + 2], nodes[0].bounds[16 + 2],
                    nodes[0].bounds[20 + 2]));
      const AABB bbox3 =
          AABB(Vec3(nodes[0].bounds[0 + 3], nodes[0].bounds[4 + 3],
                    nodes[0].bounds[8 + 3]),
               Vec3(nodes[0].bounds[12 + 3], nodes[0].bounds[16 + 3],
                    nodes[0].bounds[20 + 3]));
      return mergeAABB(bbox0, mergeAABB(bbox1, mergeAABB(bbox2, bbox3)));
    } else {
      return AABB();
    }
  }
};

}  // namespace LTRE

#endif