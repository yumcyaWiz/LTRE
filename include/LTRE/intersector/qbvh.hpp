#ifndef _LTRE_QBVH_H
#define _LTRE_QBVH_H
#include "LTRE/intersector/bvh.hpp"
#include "LTRE/intersector/intersector.hpp"

namespace LTRE {

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
    int enc = 0;
    enc |= (1 << 31);
    enc |= ((nPrims & 0xf) << 27);
    enc |= (primStart & 0x7fffffff);
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
    bool makeLeaf;
    BVH<T, strategy>::splitAABB(topAABB, this->primitives, primStart, primEnd,
                                splitAxisTop, splitIdxTop, makeLeaf);

    // compute left AABB
    const AABB leftAABB =
        BVH<T, strategy>::computeAABB(this->primitives, primStart, primEnd);

    // split left AABB
    int splitAxisLeft, splitIdxLeft;
    BVH<T, strategy>::splitAABB(leftAABB, this->primitives, primStart,
                                splitIdxTop, splitAxisLeft, splitIdxLeft,
                                makeLeaf);

    // compute right AABB
    const AABB rightAABB =
        BVH<T, strategy>::computeAABB(this->primitives, splitIdxTop, primEnd);

    // split right AABB
    int splitAxisRight, splitIdxRight;
    BVH<T, strategy>::splitAABB(rightAABB, this->primitives, splitIdxTop,
                                primEnd, splitAxisRight, splitIdxRight,
                                makeLeaf);

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

    // add parent node
    const int parentOffset = nodes.size();
    nodes.push_back(node);
    stats.nInternalNodes++;

    // set index of child0
    const int child0Offset = nodes.size();
    nodes[parentOffset].child[0] = child0Offset;

    if (nPrimsChild[0] <= 4) {
      // make leaf node
      node.child[0] = encodeLeaf(nPrimsChild[0], primStartChild[0]);
      nodes.push_back(node);
      stats.nLeafNodes++;
    } else {
      // build child0 subtree
      buildBVHNode(primStart, splitIdxLeft);
    }

    // set index of child1
    const int child1Offset = nodes.size();
    nodes[parentOffset].child[1] = child1Offset;

    // build child1 subtree
    if (nPrimsChild[1] <= 4) {
      // make leaf node
      node.child[1] = encodeLeaf(nPrimsChild[1], primStartChild[1]);
      nodes.push_back(node);
      stats.nLeafNodes++;
    } else {
      buildBVHNode(splitIdxLeft, splitIdxTop);
    }

    // set index of child2
    const int child2Offset = nodes.size();
    nodes[parentOffset].child[2] = child2Offset;

    // build child2 subtree
    if (nPrimsChild[2] <= 4) {
      // make leaf node
      node.child[2] = encodeLeaf(nPrimsChild[2], primStartChild[2]);
      nodes.push_back(node);
      stats.nLeafNodes++;
    } else {
      buildBVHNode(splitIdxTop, splitIdxRight);
    }

    // set index of child3
    const int child3Offset = nodes.size();
    nodes[parentOffset].child[3] = child3Offset;

    // build child3 subtree
    if (nPrimsChild[3] <= 4) {
      // make leaf node
      node.child[3] = encodeLeaf(nPrimsChild[3], primStartChild[3]);
      nodes.push_back(node);
      stats.nLeafNodes++;
    } else {
      buildBVHNode(splitIdxRight, primEnd);
    }
  }

 public:
  QBVH() {}
  QBVH(const std::vector<T>& primitives) : Intersector<T>(primitives) {}

  bool build() override {
    // start building bvh from root node
    buildBVHNode(0, this->primitives.size());
    stats.nNodes = stats.nInternalNodes + stats.nLeafNodes;
    spdlog::info("[BVH] nPrimitives: " +
                 std::to_string(this->primitives.size()));
    spdlog::info("[BVH] nNodes: " + std::to_string(stats.nNodes));
    spdlog::info("[BVH] nInternalNodes: " +
                 std::to_string(stats.nInternalNodes));
    spdlog::info("[BVH] nInternalNodes: " + std::to_string(stats.nLeafNodes));

    return true;
  }
  bool intersect(const Ray& ray, IntersectInfo& info) const override {
    return false;
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