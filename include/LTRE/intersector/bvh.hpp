#ifndef _LTRE_BVH_H
#define _LTRE_BVH_H
#include "LTRE/core/aabb.hpp"
#include "LTRE/intersector/intersector.hpp"

namespace LTRE {

enum class BVHSplitStrategy { CENTER, EQUAL, SAH };

template <Intersectable T, BVHSplitStrategy strategy>
class BVH : public Intersector<T> {
  // NOTE: 32Byte alighment to make node cache conscious
  struct alignas(32) BVHNode {
    AABB bbox;
    union {
      uint32_t primIndicesOffset;  // index of primIndices
      uint32_t secondChildOffset;  // index of second child
    };
    uint16_t nPrimitives{0};  // number of primitives in this node
    uint8_t axis{0};          // splitting axis(x=0, y=1, z=2)
  };

  struct BVHStatistics {
    int nNodes{0};          // number of nodes
    int nInternalNodes{0};  // number of internal nodes
    int nLeafNodes{0};      // number of leaf nodes
  };

  std::vector<BVHNode> nodes;  // node array(depth-first order)
  BVHStatistics stats;

  // add leaf node to node array
  void addLeafNode(const AABB& bbox, int primStart, int nPrims) {
    BVHNode node;
    node.bbox = bbox;
    node.primIndicesOffset = primStart;
    node.nPrimitives = nPrims;
    nodes.push_back(node);
    stats.nLeafNodes++;
  }

  // build bvh node recursively
  void buildBVHNode(int primStart, int primEnd) {
    // compute AABB
    AABB bbox;
    for (int i = primStart; i < primEnd; ++i) {
      bbox = mergeAABB(bbox, this->primitives[i].aabb());
    }

    // if there are only few primitives, make leaf node
    const int nPrims = primEnd - primStart;
    if (nPrims <= 4) {
      addLeafNode(bbox, primStart, nPrims);
      return;
    }

    // compute AABB which contains each primitive's center
    // NOTE: using bbox for splitting will cause splitting failed
    AABB splitAABB;
    for (int i = primStart; i < primEnd; ++i) {
      splitAABB = mergeAABB(splitAABB, this->primitives[i].aabb().center());
    }

    const int splitAxis = splitAABB.longestAxis();

    // splitting AABB(equal number splitting)
    int splitIdx = primStart + nPrims / 2;
    if constexpr (strategy == BVHSplitStrategy::EQUAL) {
      std::nth_element(this->primitives.begin() + primStart,
                       this->primitives.begin() + splitIdx,
                       this->primitives.begin() + primEnd,
                       [&](const auto& prim1, const auto& prim2) {
                         return prim1.aabb().center()[splitAxis] <
                                prim2.aabb().center()[splitAxis];
                       });
    } else if constexpr (strategy == BVHSplitStrategy::CENTER) {
    } else if constexpr (strategy == BVHSplitStrategy::SAH) {
    }

    // if splitting failed, make leaf node
    if (splitIdx == primStart || splitIdx == primEnd) {
      spdlog::warn("[BVH] splitting failed");
      spdlog::warn("[BVH] nPrimitives: " + std::to_string(nPrims));
      spdlog::warn("[BVH] splitAxis: " + std::to_string(splitAxis));
      spdlog::warn("[BVH] primStart: " + std::to_string(primStart));
      spdlog::warn("[BVH] splitIdx: " + std::to_string(splitIdx));
      spdlog::warn("[BVH] primEnd: " + std::to_string(primEnd));
      addLeafNode(bbox, primStart, nPrims);
      return;
    }

    // add node to node array, remember index of current node(parent node)
    const int parentOffset = nodes.size();
    BVHNode node;
    node.bbox = bbox;
    node.primIndicesOffset = primStart;
    node.axis = splitAxis;
    nodes.push_back(node);
    stats.nInternalNodes++;

    // add left children on node array
    buildBVHNode(primStart, splitIdx);

    // set index of right child on parent node
    const int secondChildOffset = nodes.size();
    nodes[parentOffset].secondChildOffset = secondChildOffset;

    // add right children on node array
    buildBVHNode(splitIdx, primEnd);
  }

  // traverse bvh recursively
  bool intersectNode(int nodeIdx, const Ray& ray, const Vec3& dirInv,
                     const int dirInvSign[3], IntersectInfo& info) const {
    bool hit = false;
    const BVHNode& node = nodes[nodeIdx];

    if (node.bbox.intersect(ray, dirInv, dirInvSign)) {
      // leaf node
      if (node.nPrimitives > 0) {
        // test intersection with all primitives in this node
        const int primEnd = node.primIndicesOffset + node.nPrimitives;
        for (int i = node.primIndicesOffset; i < primEnd; ++i) {
          if (this->primitives[i].intersect(ray, info)) {
            hit = true;
            ray.tmax = info.t;
          }
        }
      }
      // internal node
      else {
        // test intersection with child node
        // use splitting axis heuristics for early termination
        if (dirInvSign[node.axis] == 0) {
          hit |= intersectNode(nodeIdx + 1, ray, dirInv, dirInvSign, info);
          hit |= intersectNode(node.secondChildOffset, ray, dirInv, dirInvSign,
                               info);
        } else {
          hit |= intersectNode(node.secondChildOffset, ray, dirInv, dirInvSign,
                               info);
          hit |= intersectNode(nodeIdx + 1, ray, dirInv, dirInvSign, info);
        }
      }
    }

    return hit;
  }

 public:
  BVH() {}
  BVH(const std::vector<T>& primitives) : Intersector<T>(primitives) {}

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

  // number of nodes
  int nNodes() const { return stats.nNodes; }
  // number of internal nodes
  int nInternalNodes() const { return stats.nInternalNodes; }
  // number of leaf nodes
  int nLeafNodes() const { return stats.nLeafNodes; }

  AABB aabb() const {
    if (nodes.size() > 0) {
      return nodes[0].bbox;
    } else {
      return AABB();
    }
  }

  bool intersect(const Ray& ray, IntersectInfo& info) const {
    // precompute ray's inversed direction, sign of direction
    const Vec3 dirInv = 1.0f / ray.direction;
    int dirInvSign[3];
    for (int i = 0; i < 3; ++i) {
      dirInvSign[i] = dirInv[i] > 0 ? 0 : 1;
    }
    // traverse from root node
    return intersectNode(0, ray, dirInv, dirInvSign, info);
  }
};

}  // namespace LTRE

#endif