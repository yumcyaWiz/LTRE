#ifndef _LTRE_IO_H
#define _LTRE_IO_H
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "spdlog/spdlog.h"
#include "stb_image.h"

//
#include "LTRE/core/image.hpp"

namespace LTRE {

class ImageLoader {
 private:
  static Image<Vec3> loadJPG(const std::filesystem::path& filepath) {
    Image<Vec3> ret;

    // load image with stb_image
    int width, height, channels;
    unsigned char* img = stbi_load(filepath.generic_string().c_str(), &width,
                                   &height, &channels, 3);
    if (!img) {
      spdlog::error("[ImageLoader] failed to load " + filepath.string());
      std::exit(EXIT_FAILURE);
    }

    // make float image
    ret.resize(width, height);
    for (int j = 0; j < height; ++j) {
      for (int i = 0; i < width; ++i) {
        constexpr float divider = 1.0f / 255.0f;
        // sRGB to linear RGB transform
        const float R = std::pow(img[3 * i + 3 * width * j] * divider, 2.2f);
        const float G =
            std::pow(img[3 * i + 3 * width * j + 1] * divider, 2.2f);
        const float B =
            std::pow(img[3 * i + 3 * width * j + 2] * divider, 2.2f);
        ret.setPixel(i, j, Vec3(R, G, B));
      }
    }

    // free stb image
    stbi_image_free(img);

    return ret;
  }

  static Image<Vec3> loadPNG(const std::filesystem::path& filepath) {
    Image<Vec3> ret;

    // load image with stb_image
    int width, height, channels;
    unsigned char* img = stbi_load(filepath.generic_string().c_str(), &width,
                                   &height, &channels, 3);
    if (!img) {
      spdlog::error("[ImageLoader] failed to load " + filepath.string());
      std::exit(EXIT_FAILURE);
    }

    // make float image
    ret.resize(width, height);
    for (int j = 0; j < height; ++j) {
      for (int i = 0; i < width; ++i) {
        constexpr float divider = 1.0f / 255.0f;
        // sRGB to linear RGB transform
        const float R = std::pow(img[3 * i + 3 * width * j] * divider, 2.2f);
        const float G =
            std::pow(img[3 * i + 3 * width * j + 1] * divider, 2.2f);
        const float B =
            std::pow(img[3 * i + 3 * width * j + 2] * divider, 2.2f);
        ret.setPixel(i, j, Vec3(R, G, B));
      }
    }

    // free stb image
    stbi_image_free(img);

    return ret;
  }

  static Image<Vec3> loadHDR(const std::filesystem::path& filepath) {
    Image<Vec3> ret;

    // load image with stb_image
    int width, height, channels;
    float* img = stbi_loadf(filepath.generic_string().c_str(), &width, &height,
                            &channels, 3);
    if (!img) {
      spdlog::error("[ImageLoader] failed to load " + filepath.string());
      std::exit(EXIT_FAILURE);
    }

    // make float image
    ret.resize(width, height);
    for (int j = 0; j < height; ++j) {
      for (int i = 0; i < width; ++i) {
        const float R = img[3 * i + 3 * width * j];
        const float G = img[3 * i + 3 * width * j + 1];
        const float B = img[3 * i + 3 * width * j + 2];
        ret.setPixel(i, j, Vec3(R, G, B));
      }
    }

    // free stb image
    stbi_image_free(img);

    return ret;
  }

 public:
  static Image<Vec3> loadImage(const std::filesystem::path& filepath) {
    spdlog::info("[ImageLoader] loading {}", filepath.string());

    const std::string ext = filepath.extension();
    if (ext == ".jpg") {
      return loadJPG(filepath);
    } else if (ext == ".png") {
      return loadPNG(filepath);
    } else if (ext == ".hdr") {
      return loadHDR(filepath);
    } else {
      spdlog::error("unsupported image format");
      std::exit(EXIT_FAILURE);
    }
  }
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