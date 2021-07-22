#ifndef _LTRE_IO_H
#define _LTRE_IO_H
#include <filesystem>
#include <fstream>
#include <iostream>

#include "LTRE/core/image.hpp"
#include "LTRE/math/vec3.hpp"

namespace LTRE {

class ImageLoader {
 private:
  static Image<Vec3> loadJPG(const std::filesystem::path& filepath);
  static Image<Vec3> loadPNG(const std::filesystem::path& filepath);
  static Image<Vec3> loadHDR(const std::filesystem::path& filepath);

 public:
  static Image<Vec3> loadImage(const std::filesystem::path& filepath);
};

class ImageWriter {
 private:
  template <typename T>
  static void writePPM(const Image<T>& image,
                       const std::filesystem::path& filepath) {
    std::ofstream file(filepath.generic_string());
    if (!file) {
      spdlog::error("[Image] failed to open {0}", filepath.string());
      return;
    }

    file << "P3" << std::endl;
    file << image.getWidth() << " " << image.getHeight() << std::endl;
    file << "255" << std::endl;
    for (unsigned int j = 0; j < image.getHeight(); ++j) {
      for (unsigned int i = 0; i < image.getWidth(); ++i) {
        const T c = image.getPixel(i, j);

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

 public:
  template <typename T>
  static void writeImage(const Image<T>& image,
                         const std::filesystem::path& filepath) {
    spdlog::info("[ImageWriter] writing {}", filepath.string());

    const std::string ext = filepath.extension();
    if (ext == ".ppm") {
      writePPM(image, filepath);
    } else {
      spdlog::error("unsupported image format");
      std::exit(EXIT_FAILURE);
    }
  }
};

}  // namespace LTRE

#endif