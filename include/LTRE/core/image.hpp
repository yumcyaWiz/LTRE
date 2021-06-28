#ifndef _LTRE_IMAGE_H
#define _LTRE_IMAGE_H
#include <fstream>
#include <iostream>
#include <string>
#include <type_traits>

#include "spdlog/spdlog.h"

namespace LTRE {

template <typename T>
class Image {
 private:
  unsigned int width;
  unsigned int height;
  std::vector<T> image;

 public:
  Image() {}
  Image(unsigned int width, unsigned int height)
      : width(width), height(height) {
    image.resize(width * height);
  }

  unsigned int getWidth() const { return width; }
  unsigned int getHeight() const { return height; }

  void resize(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;
    image.resize(width * height);
  }

  T getPixel(unsigned int i, unsigned int j) const {
    using namespace std::string_literals;
    if (i >= width || j >= height) {
      spdlog::error("[Image] pixel index out of bounds "s + "(" +
                    std::to_string(i) + ", " + std::to_string(j) + ")");
      std::exit(EXIT_FAILURE);
    }
    return image[i + width * j];
  }
  void setPixel(unsigned int i, unsigned int j, const T& value) {
    using namespace std::string_literals;
    if (i >= width || j >= height) {
      spdlog::error("[Image] pixel index out of bounds "s + "(" +
                    std::to_string(i) + ", " + std::to_string(j) + ")");
      std::exit(EXIT_FAILURE);
    }
    image[i + width * j] = value;
  }

  void writePPM(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
      spdlog::error("[Image] failed to open " + filename);
    }

    file << "P3" << std::endl;
    file << width << " " << height << std::endl;
    file << "255" << std::endl;
    for (unsigned int j = 0; j < height; ++j) {
      for (unsigned int i = 0; i < width; ++i) {
        const T c = getPixel(i, j);

        // TODO: use concept to restrict template parameter?
        if constexpr (std::is_floating_point_v<T>) {
          file << std::clamp(static_cast<int>(255.0f * c), 0, 255) << " ";
          file << std::clamp(static_cast<int>(255.0f * c), 0, 255) << " ";
          file << std::clamp(static_cast<int>(255.0f * c), 0, 255) << std::endl;
        } else if constexpr (T::nComponents == 2) {
          file << std::clamp(static_cast<int>(255.0f * c[0]), 0, 255) << " ";
          file << std::clamp(static_cast<int>(255.0f * c[1]), 0, 255) << " ";
          file << std::clamp(static_cast<int>(255.0f * 0), 0, 255) << std::endl;
        } else if constexpr (T::nComponents == 3) {
          file << std::clamp(static_cast<int>(255.0f * c[0]), 0, 255) << " ";
          file << std::clamp(static_cast<int>(255.0f * c[1]), 0, 255) << " ";
          file << std::clamp(static_cast<int>(255.0f * c[2]), 0, 255)
               << std::endl;
        }
      }
    }
    file.close();
  }

  // compute average pixel value of image
  T average() const {
    T sum{0};
    for (unsigned int j = 0; j < height; ++j) {
      for (unsigned int i = 0; i < width; ++i) {
        sum += getPixel(i, j);
      }
    }
    return sum / (width * height);
  }
};

void gammaCorrection(Image<Vec3>& image) {
  for (unsigned int j = 0; j < image.getHeight(); ++j) {
    for (unsigned int i = 0; i < image.getWidth(); ++i) {
      const Vec3 c = image.getPixel(i, j);
      constexpr float power = 1.0f / 2.2f;
      image.setPixel(i, j, pow(c, power));
    }
  }
}

}  // namespace LTRE

#endif