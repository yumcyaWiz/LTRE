#ifndef _LTRE_QBVH_H
#define _LTRE_QBVH_H
#include "LTRE/intersector/bvh.hpp"
#include "LTRE/intersector/intersector.hpp"

namespace LTRE {

template <Intersectable T, BVHSplitStrategy strategy>
class QBVH {
 private:
  struct alignas(128) BVHNode {
    float bounds[4 * 2 * 3];
    int child[4];
    int axisTop;
    int axisLeft;
    int axisRight;
  };

  struct BVHStatistics {
    int nNodes{0};
    int nInternalNodes{0};
    int nLeafNodes{0};
  };

  std::vector<BVHNode> nodes;
  BVHStatistics stats;

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

  AABB computeAABB(int primStart, int primEnd) {
    AABB ret;
    for (int i = primStart; i < primEnd; ++i) {
      ret = mergeAABB(ret, this->primitives[i].aabb());
    }
    return ret;
  }

  AABB computeCentroidAABB(int primStart, int primEnd) {
    AABB ret;
    for (int i = primStart; i < primEnd; ++i) {
      ret = mergeAABB(ret, this->primitives[i].aabb().center());
    }
    return ret;
  }

  void splitAABB(int primStart, int primEnd, int& splitAxis, int& splitIdx,
                 bool& makeLeaf) {
    // if there are only few primitives, make leaf node
    const int nPrims = primEnd - primStart;
    if (nPrims <= 2) {
      makeLeaf = true;
      return;
    }

    // compute split axis
    const AABB splitAABB = computeCentroidAABB(primStart, primEnd);
    splitAxis = splitAABB.longestAxis();

    // split AABB
    splitIdx = primStart;
    if constexpr (strategy == BVHSplitStrategy::EQUAL) {
      splitIdx = primStart + nPrims / 2;
      std::nth_element(this->primitives.begin() + primStart,
                       this->primitives.begin() + splitIdx,
                       this->primitives.begin() + primEnd,
                       [&](const auto& prim1, const auto& prim2) {
                         return prim1.aabb().center()[splitAxis] <
                                prim2.aabb().center()[splitAxis];
                       });
    } else if constexpr (strategy == BVHSplitStrategy::CENTER) {
      const float center = splitAABB.center()[splitAxis];
      splitIdx =
          std::partition(this->primitives.begin() + primStart,
                         this->primitives.begin() + primEnd,
                         [&](const auto& prim) {
                           return prim.aabb().center()[splitAxis] < center;
                         }) -
          this->primitives.begin();
    } else if constexpr (strategy == BVHSplitStrategy::SAH) {
      constexpr int SAH_NUM_BINS = 36;

      struct SAHBin {
        int nPrimitives;
        AABB bounds;
        SAHBin() : nPrimitives{0}, bounds{} {}
      };
      SAHBin bins[SAH_NUM_BINS];

      // populate SAHBin
      for (int i = primStart; i < primEnd; ++i) {
        // compute SAH bin index
        const float primPos = this->primitives[i].aabb().center()[splitAxis];
        const float parentStartPos = nodeAABB.bounds[0][splitAxis];
        const float parentEndPos = nodeAABB.bounds[1][splitAxis];
        int binIdx = SAH_NUM_BINS * (primPos - parentStartPos) /
                     (parentEndPos - parentStartPos);
        if (binIdx == SAH_NUM_BINS) binIdx--;

        bins[binIdx].nPrimitives++;
        bins[binIdx].bounds =
            mergeAABB(bins[binIdx].bounds, this->primitives[i].aabb());
      }

      // compute SAH cost
      constexpr float traverseCost = 0.125f;
      constexpr float intersectCost = 1.0f;
      float costs[SAH_NUM_BINS];
      for (int i = 0; i < SAH_NUM_BINS; ++i) {
        int nPrimitives1 = 0;
        AABB bounds1;
        for (int j = 0; j <= i; ++j) {
          nPrimitives1 += bins[j].nPrimitives;
          bounds1 = mergeAABB(bounds1, bins[j].bounds);
        }

        int nPrimitives2 = 0;
        AABB bounds2;
        for (int j = i + 1; j < SAH_NUM_BINS; ++j) {
          nPrimitives2 += bins[j].nPrimitives;
          bounds2 = mergeAABB(bounds2, bins[j].bounds);
        }

        costs[i] = traverseCost +
                   (nPrimitives1 * intersectCost * bounds1.surfaceArea() +
                    nPrimitives2 * intersectCost * bounds2.surfaceArea()) /
                       nodeAABB.surfaceArea();
      }

      // find minimum SAH cost
      int minCostIdx = 0;
      float minCost = costs[0];
      for (int i = 1; i < SAH_NUM_BINS; ++i) {
        if (costs[i] < minCost) {
          minCostIdx = i;
          minCost = costs[i];
        }
      }

      const float leafCost = nPrims * intersectCost;
      if (minCost < leafCost) {
        // split AABB
        const float parentStartPos = nodeAABB.bounds[0][splitAxis];
        const float parentEndPos = nodeAABB.bounds[1][splitAxis];
        splitIdx = std::partition(
                       this->primitives.begin() + primStart,
                       this->primitives.begin() + primEnd,
                       [&](const auto& prim) {
                         const float primPos = prim.aabb().center()[splitAxis];
                         int binIdx = SAH_NUM_BINS *
                                      (primPos - parentStartPos) /
                                      (parentEndPos - parentStartPos);
                         if (binIdx == SAH_NUM_BINS) binIdx--;
                         return binIdx <= minCostIdx;
                       }) -
                   this->primitives.begin();
      } else {
        // make leaf node
        makeLeaf = true;
        return;
      }
    }
  }

  // build bvh node recursively
  void buildBVHNode(int primStart, int primEnd) {}

  static bool isLeaf(int child) { return ((child & 0x80000000) >> 31) == 1; }

 public:
  QBVH() {}
  QBVH(const std::vector<T>& primitives) : Intersector<T>(primitives) {}

  bool build() override { return false; }
  bool intersect(const Ray& ray, IntersectInfo& info) const override {
    return false;
  }
};

}  // namespace LTRE

#endif