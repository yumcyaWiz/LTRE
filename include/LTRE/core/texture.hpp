#ifndef _LTRE_TEXTURE_H
#define _LTRE_TEXTURE_H
#include <filesystem>

#include "spdlog/spdlog.h"
//
#include "LTRE/core/image.hpp"
#include "LTRE/core/io.hpp"
#include "LTRE/core/types.hpp"

namespace LTRE {

template <typename T>
class Texture {
 protected:
  Vec2 calcTexCoords(const SurfaceInfo& info) const {
    // GL_REPEAT
    float u = info.uv[0];
    if (u > 1) {
      u = std::fmod(u, 1.0f);
    } else if (u < 0) {
      u = 1.0f + std::fmod(u, 1.0f);
    }

    float v = info.uv[1];
    if (v > 1) {
      v = std::fmod(v, 1.0f);
    } else if (v < 0) {
      v = 1.0f + std::fmod(v, 1.0f);
    }

    return Vec2(u, v);
  }

 public:
  // sample color from texture
  virtual T sample(const SurfaceInfo& info) const = 0;

  // average color of texture
  virtual T average() const = 0;
};

template <typename T>
class UniformTexture : public Texture<T> {
 private:
  T value;

 public:
  UniformTexture(const T& value) : value(value) {}

  T sample([[maybe_unused]] const SurfaceInfo& info) const override {
    return value;
  };

  T average() const override { return value; }
};

class ImageTexture : public Texture<Vec3> {
 private:
  Image<Vec3> image;
  std::filesystem::path filepath;

 public:
  ImageTexture(const std::filesystem::path& filepath) : filepath(filepath) {
    image = ImageLoader::loadImage(filepath);
  }

  std::filesystem::path getFilepath() const { return filepath; }

  Vec3 sample(const SurfaceInfo& info) const override {
    const Vec2 uv = calcTexCoords(info);
    const int width = image.getWidth();
    const int height = image.getHeight();
    const int i = std::clamp(static_cast<int>(width * uv[0]), 0, width - 1);
    const int j = std::clamp(static_cast<int>(height * uv[1]), 0, height - 1);
    return image.getPixel(i, j);
  }

  Vec3 average() const override { return image.average(); }
};

}  // namespace LTRE

#endif