#ifndef _LTRE_FILM_H
#define _LTRE_FILM_H
#include <cassert>
#include <fstream>
#include <iostream>

#include "LTRE/math/vec3.hpp"

namespace LTRE {

class Film {
 private:
  unsigned int width;
  unsigned int height;
  float* pixels;

 public:
  Film(unsigned int width, unsigned int height) : width(width), height(height) {
    pixels = new float[3 * width * height];
  }
  ~Film() { delete[] pixels; }

  Vec3 getPixel(unsigned int i, unsigned int j) const {
    assert(i >= 0 && i < width);
    assert(j >= 0 && j < height);
    return Vec3(pixels[3 * i + 3 * width * j],
                pixels[3 * i + 3 * width * j + 1],
                pixels[3 * i + 3 * width * j + 2]);
  }

  void setPixel(unsigned int i, unsigned int j, const Vec3& c) {
    assert(i >= 0 && i < width);
    assert(j >= 0 && j < height);
    for (int k = 0; k < 3; ++k) {
      pixels[3 * i + 3 * width * j + k] = c[k];
    }
  }

  void gammaCorrection() const {
    for (int j = 0; j < height; ++j) {
      for (int i = 0; i < width; ++i) {
        pixels[3 * i + 3 * width * j] =
            std::pow(pixels[3 * i + 3 * width * j], 1.0f / 2.2f);
      }
    }
  }

  void writePPM(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
      std::cerr << "failed to open " << filename << std::endl;
    }

    file << "P3" << std::endl;
    file << width << " " << height << std::endl;
    file << "255" << std::endl;
    for (int j = 0; j < height; ++j) {
      for (int i = 0; i < width; ++i) {
        file << pixels[3 * i + 3 * width * j] << " ";
        file << pixels[3 * i + 3 * width * j + 1] << " ";
        file << pixels[3 * i + 3 * width * j + 2] << " ";
        file << std::endl;
      }
    }
    file.close();
  }
};

}  // namespace LTRE

#endif