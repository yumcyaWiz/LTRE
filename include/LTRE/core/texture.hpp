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
  unsigned char* image;
  std::filesystem::path filepath;

  void loadImage(const std::filesystem::path& filepath) {
    // load image with stb_image
    int width, height, channels;
    unsigned char* image = stbi_load(filepath.generic_string().c_str(), &width,
                                     &height, &channels, 3);
    if (!image) {
      spdlog::error("failed to open " + filepath.string());
      return;
    }
  }

 public:
  ImageTexture(const std::filesystem::path& filepath) : filepath(filepath) {
    loadImage(filepath);
  }

  ~ImageTexture() override {
    if (image) {
      stbi_image_free(image);
    }
  }

  std::filesystem::path getFilepath() const { return filepath; }

  Vec3 sample(const IntersectInfo& info) const override {
    const int i = std::clamp(static_cast<int>(width * info.uv[0]), 0, width);
    const int j = std::clamp(static_cast<int>(height * info.uv[1]), 0, height);
    const int idx = 3 * i + 3 * width * j;
    return Vec3(image[idx] / 255.0f, image[idx + 1] / 255.0f,
                image[idx + 2] / 255.0f);
  }
};

}  // namespace LTRE

#endif