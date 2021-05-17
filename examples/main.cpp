#include "LTRE/core/film.hpp"
#include "LTRE/core/intersect-info.hpp"
#include "LTRE/core/ray.hpp"
#include "LTRE/math/vec2.hpp"
#include "LTRE/math/vec3.hpp"
#include "LTRE/shape/sphere.hpp"

using namespace LTRE;

int main() {
  const int width = 512;
  const int height = 512;

  Film film(width, height);
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      film.setPixel(i, j, Vec3(i / 512.0f, j / 512.0f, 1.0f));
    }
  }

  film.writePPM("output.ppm");
}