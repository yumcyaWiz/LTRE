#include "LTRE/camera/pinhole-camera.hpp"
#include "LTRE/core/film.hpp"
#include "LTRE/core/primitive.hpp"
#include "LTRE/core/ray.hpp"
#include "LTRE/intersector/linear-intersector.hpp"
#include "LTRE/shape/sphere.hpp"

using namespace LTRE;

int main() {
  const int width = 512;
  const int height = 512;
  Film film(width, height);

  PinholeCamera camera(Vec3(0, 1, 5), Vec3(0, 0, -1));

  const auto sphere1 = std::make_shared<Sphere>(Vec3(0, -1001, 0), 1000);
  const auto sphere2 = std::make_shared<Sphere>(Vec3(0), 1);
  auto intersector = LinearIntersector();
  intersector.addPrimitive(Primitive(sphere1));
  intersector.addPrimitive(Primitive(sphere2));

  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      const float u = (2.0f * i - width) / height;
      const float v = (2.0f * j - height) / height;

      Ray ray;
      float pdf;
      if (camera.sampleRay(u, v, ray, pdf)) {
        IntersectInfo info;
        if (intersector.intersect(ray, info)) {
          film.setPixel(i, j, 0.5f * info.hitNormal + 0.5f);
        } else {
          film.setPixel(i, j, Vec3(0));
        }
      }
    }
  }

  film.writePPM("output.ppm");
}