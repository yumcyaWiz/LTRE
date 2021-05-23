#include "LTRE/camera/pinhole-camera.hpp"
#include "LTRE/core/film.hpp"
#include "LTRE/core/ray.hpp"
#include "LTRE/intersector/linear-intersector.hpp"
#include "LTRE/math/vec2.hpp"
#include "LTRE/math/vec3.hpp"

using namespace LTRE;

int main() {
  const int width = 512;
  const int height = 512;

  PinholeCamera camera(Vec3(0, 0, 3), Vec3(0, 0, -1));

  Film film(width, height);
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      const float u = (2.0f * i - width) / height;
      const float v = (2.0f * j - height) / height;

      Ray ray;
      float pdf;
      if (camera.sampleRay(u, v, ray, pdf)) {
        film.setPixel(i, j, 0.5f * ray.direction + 0.5f);
      }
    }
  }

  film.writePPM("output.ppm");
}