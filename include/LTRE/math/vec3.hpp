#ifndef _LTRE_VEC3_H
#define _LTRE_VEC3_H
#include <cassert>
#include <cmath>

namespace LTRE {

struct Vec3 {
  float v[3];

  explicit constexpr Vec3() : v{0, 0, 0} {}
  explicit constexpr Vec3(float x) : v{x, x, x} {}
  explicit constexpr Vec3(float x, float y, float z) : v{x, y, z} {}

  constexpr const float& operator[](int i) const {
    assert(i >= 0 && i <= 3);
    return v[i];
  }
  constexpr float& operator[](int i) {
    assert(i >= 0 && i <= 3);
    return v[i];
  }

  constexpr Vec3 operator-() const { return Vec3(-v[0], -v[1], -v[2]); }

  constexpr Vec3& operator+=(const Vec3& v) {
    for (int i = 0; i < 3; ++i) {
      this->v[i] += v[i];
    }
    return *this;
  }
  constexpr Vec3& operator-=(const Vec3& v) {
    for (int i = 0; i < 3; ++i) {
      this->v[i] -= v[i];
    }
    return *this;
  }
  constexpr Vec3& operator*=(const Vec3& v) {
    for (int i = 0; i < 3; ++i) {
      this->v[i] *= v[i];
    }
    return *this;
  }
  constexpr Vec3& operator/=(const Vec3& v) {
    for (int i = 0; i < 3; ++i) {
      this->v[i] /= v[i];
    }
    return *this;
  }
};

inline constexpr Vec3 operator+(const Vec3& v1, const Vec3& v2) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] + v2[i];
  }
  return ret;
}
inline constexpr Vec3 operator+(const Vec3& v1, float k) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] + k;
  }
  return ret;
}
inline constexpr Vec3 operator+(float k, const Vec3& v2) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = k + v2[i];
  }
  return ret;
}

inline constexpr Vec3 operator-(const Vec3& v1, const Vec3& v2) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] - v2[i];
  }
  return ret;
}
inline constexpr Vec3 operator-(const Vec3& v1, float k) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] - k;
  }
  return ret;
}
inline constexpr Vec3 operator-(float k, const Vec3& v2) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = k - v2[i];
  }
  return ret;
}

inline constexpr Vec3 operator*(const Vec3& v1, const Vec3& v2) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] * v2[i];
  }
  return ret;
}
inline constexpr Vec3 operator*(const Vec3& v1, float k) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] * k;
  }
  return ret;
}
inline constexpr Vec3 operator*(float k, const Vec3& v2) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = k * v2[i];
  }
  return ret;
}

inline constexpr Vec3 operator/(const Vec3& v1, const Vec3& v2) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] / v2[i];
  }
  return ret;
}
inline constexpr Vec3 operator/(const Vec3& v1, float k) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v1[i] / k;
  }
  return ret;
}
inline constexpr Vec3 operator/(float k, const Vec3& v2) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = k / v2[i];
  }
  return ret;
}

inline constexpr float dot(const Vec3& v1, const Vec3& v2) {
  float ret = 0;
  for (int i = 0; i < 3; ++i) {
    ret += v1[i] * v2[i];
  }
  return ret;
}
inline constexpr Vec3 cross(const Vec3& v1, const Vec3& v2) {
  return Vec3(v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2],
              v1[0] * v2[1] - v1[1] * v2[0]);
}

inline constexpr float length(const Vec3& v) { return std::sqrt(dot(v, v)); }
inline constexpr float length2(const Vec3& v) { return dot(v, v); }

}  // namespace LTRE

#endif