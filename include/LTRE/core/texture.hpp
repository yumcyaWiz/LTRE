#ifndef _LTRE_TEXTURE_H
#define _LTRE_TEXTURE_H
#include <filesystem>

#include "spdlog/spdlog.h"
#include "stb_image.h"
//
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
  virtual ~Texture() {}
  virtual T sample(const IntersectInfo& info) const = 0;
};

template <typename T>
class UniformTexture : public Texture<T> {
 private:
  T value;

 public:
  UniformTexture(const T& value) : value(value) {}

  T sample(const IntersectInfo& info) const override { return value; };
};

class ImageTexture : public Texture<Vec3> {
 private:
  int width;
  int height;
  float* image;
  std::filesystem::path filepath;

  void loadImage(const std::filesystem::path& filepath) {
    // load image with stb_image
    int _channels;
    unsigned char* img = stbi_load(filepath.generic_string().c_str(), &width,
                                   &height, &_channels, 3);
    if (!img) {
      spdlog::error("failed to open " + filepath.string());
      return;
    }

    // make float image
    this->image = new float[3 * width * height];
    for (int i = 0; i < 3 * width * height; ++i) {
      constexpr float divider = 1.0f / 255.0f;
      this->image[i] = img[i] * divider;
    }

    // free stb image
    stbi_image_free(img);
  }

 public:
  ImageTexture(const std::filesystem::path& filepath) : filepath(filepath) {
    loadImage(filepath);
  }

  ~ImageTexture() override { delete[] image; }

  std::filesystem::path getFilepath() const { return filepath; }

  Vec3 sample(const IntersectInfo& info) const override {
    const Vec2 uv = calcTexCoords(info);
    const int i = std::clamp(static_cast<int>(width * uv[0]), 0, width);
    const int j = std::clamp(static_cast<int>(height * uv[1]), 0, height);
    const int idx = 3 * i + 3 * width * j;
    return Vec3(image[idx], image[idx + 1], image[idx + 2]);
  }
};

}  // namespace LTRE

#endif