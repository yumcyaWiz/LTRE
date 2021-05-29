#ifndef _LTRE_VEC3_H
#define _LTRE_VEC3_H
#include <cassert>
#include <cmath>
#include <iostream>

namespace LTRE {

struct Vec3 {
  float v[3];

  explicit constexpr Vec3() : v{0, 0, 0} {}
  explicit constexpr Vec3(float x) : v{x, x, x} {}
  explicit constexpr Vec3(float x, float y, float z) : v{x, y, z} {}

  constexpr bool operator==(const Vec3& v) const {
    return this->v[0] == v[0] && this->v[1] == v[1] && this->v[2] == v[2];
  }

  constexpr const float& operator[](int i) const {
    assert(i >= 0 && i < 3);
    return v[i];
  }
  constexpr float& operator[](int i) {
    assert(i >= 0 && i < 3);
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
  constexpr Vec3& operator*=(float k) {
    for (int i = 0; i < 3; ++i) {
      this->v[i] *= k;
    }
    return *this;
  }
  constexpr Vec3& operator/=(const Vec3& v) {
    for (int i = 0; i < 3; ++i) {
      this->v[i] /= v[i];
    }
    return *this;
  }
  constexpr Vec3& operator/=(float k) {
    for (int i = 0; i < 3; ++i) {
      this->v[i] /= k;
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

inline constexpr Vec3 normalize(const Vec3& v) { return v / length(v); }

inline std::ostream& operator<<(std::ostream& stream, const Vec3& v) {
  stream << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
  return stream;
}

inline constexpr Vec3 worldToLocal(const Vec3& v, const Vec3& lx,
                                   const Vec3& ly, const Vec3& lz) {
  return Vec3(dot(v, lx), dot(v, ly), dot(v, lz));
}

inline constexpr Vec3 localToWorld(const Vec3& v, const Vec3& lx,
                                   const Vec3& ly, const Vec3& lz) {
  Vec3 ret;
  for (int i = 0; i < 3; ++i) {
    ret[i] = v[0] * lx[i] + v[1] * ly[i] + v[2] * lz[i];
  }
  return ret;
}

inline constexpr Vec3 sphericalToCartesian(float theta, float phi) {
  const float cosTheta = std::cos(theta);
  const float sinTheta = std::sin(theta);
  return Vec3(std::cos(phi) * sinTheta, cosTheta, std::sin(phi) * sinTheta);
}

inline void orthonormalBasis(const Vec3& n, Vec3& t, Vec3& b) {
  if (std::abs(n[1]) < 0.9f) {
    t = normalize(cross(n, Vec3(0, 1, 0)));
  } else {
    t = normalize(cross(n, Vec3(0, 0, -1)));
  }
  b = normalize(cross(t, n));
}

}  // namespace LTRE

#endif