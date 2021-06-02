#ifndef _LTRE_IMAGE_H
#define _LTRE_IMAGE_H
#include <fstream>
#include <iostream>
#include <type_traits>

namespace LTRE {

template <typename T>
class Image {
 private:
  unsigned int width;
  unsigned int height;
  std::vector<T> image;

 public:
  Image(unsigned int width, unsigned int height)
      : width(width), height(height) {
    image.resize(width * height);
  }

  unsigned int getWidth() const { return width; }
  unsigned int getHeight() const { return height; }

  T getPixel(unsigned int i, unsigned int j) const {
    assert(i < width);
    assert(j < height);
    return image[i + width * j];
  }
  void setPixel(unsigned int i, unsigned int j, const T& value) {
    assert(i < width);
    assert(j < height);
    image[i + width * j] = value;
  }

  void writePPM(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
      std::cerr << "failed to open " << filename << std::endl;
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