#ifndef _LTRE_TEXTURE_H
#define _LTRE_TEXTURE_H

#include "LTRE/math/vec2.hpp"

namespace LTRE {

template <typename T>
class Texture {
 public:
  T getValue(const Vec2& uv) const = 0;
};

template <typename T>
class UniformTexture {
 private:
  T value;

 public:
  UniformTexture(const T& value) : value(value) {}

  T getValue(const Vec2& uv) const override { return value; };
};

template <typename T>
class ImageTexture {
 private:
  int width;
  int height;
  T* pixels;

 public:
  ImageTexture(const std::string& filename) {}
  ~ImageTexture() { delete[] pixels; }

  T getValue(const Vec2& uv) const override {
    const int i = std::clamp(static_cast<int>(width * uv[0]), 0, width);
    const int j = std::clamp(static_cast<int>(height * uv[1]), 0, height);
    return pixels[i + width * j];
  }
};

}  // namespace LTRE

#endif