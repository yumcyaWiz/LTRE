#ifndef _LTRE_CONSTANT_H
#define _LTRE_CONSTANT_H
#include <limits>

namespace LTRE {

constexpr float PI = 3.14159265359f;
constexpr float PI_MUL_2 = 2.0f * PI;
constexpr float PI_MUL_4 = 4.0f * PI;
constexpr float PI_DIV_2 = 0.5f * PI;
constexpr float PI_DIV_4 = 0.25f * PI;
constexpr float PI_INV = 1.0f / PI;
constexpr float PI_MUL_2_INV = 1.0f / PI_MUL_2;
constexpr float PI_MUL_4_INV = 1.0f / PI_MUL_4;

constexpr float EPS = std::numeric_limits<float>::min();
constexpr float RAY_EPS = 0.001f;

}  // namespace LTRE

#endif