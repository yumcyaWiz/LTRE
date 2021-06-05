#ifndef _LTRE_TEXTURE_H
#define _LTRE_TEXTURE_H
#include <filesystem>

#include "spdlog/spdlog.h"
#include "stb_image.h"
//
#include "LTRE/core/image.hpp"
#include "LTRE/core/intersect-info.hpp"

namespace LTRE {

template <typename T>
class Texture {
 protected:
  Vec2 calcTexCoords(const IntersectInfo& info) const {
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
  virtual T sample(const IntersectInfo& info) const = 0;
};

template <typename T>
class UniformTexture : public Texture<T> {
 private:
  T value;

 public:
  UniformTexture(const T& value) : value(value) {}

  T sample([[maybe_unused]] const IntersectInfo& info) const override {
    return value;
  };
};

class ImageTexture : public Texture<Vec3> {
 private:
  Image<Vec3> image;
  std::filesystem::path filepath;

  void loadImage(const std::filesystem::path& filepath) {
    spdlog::info("[ImageTexture] loading " + filepath.string());
    // load image with stb_image
    int width, height, channels;
    unsigned char* img = stbi_load(filepath.generic_string().c_str(), &width,
                                   &height, &channels, 3);
    if (!img) {
      spdlog::error("[ImageTexture] failed to load " + filepath.string());
      std::exit(EXIT_FAILURE);
    }

    // make float image
    this->image.resize(width, height);
    for (int j = 0; j < height; ++j) {
      for (int i = 0; i < width; ++i) {
        constexpr float divider = 1.0f / 255.0f;
        const float R = img[3 * i + 3 * width * j] * divider;
        const float G = img[3 * i + 3 * width * j + 1] * divider;
        const float B = img[3 * i + 3 * width * j + 2] * divider;
        this->image.setPixel(i, j, Vec3(R, G, B));
      }
    }

    // free stb image
    stbi_image_free(img);
  }

 public:
  ImageTexture(const std::filesystem::path& filepath) : filepath(filepath) {
    loadImage(filepath);
  }

  std::filesystem::path getFilepath() const { return filepath; }

  Vec3 sample(const IntersectInfo& info) const override {
    const Vec2 uv = calcTexCoords(info);
    const int width = image.getWidth();
    const int height = image.getHeight();
    const int i = std::clamp(static_cast<int>(width * uv[0]), 0, width - 1);
    const int j = std::clamp(static_cast<int>(height * uv[1]), 0, height - 1);
    return image.getPixel(i, j);
  }
};

}  // namespace LTRE

#endif