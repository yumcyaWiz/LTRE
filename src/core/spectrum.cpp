#include "LTRE/core/spectrum.hpp"

namespace LTRE {

// http://brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
// sRGB D65
Vec3 Spectrum::RGB2XYZ(const Vec3& rgb) {
  return Vec3(0.4124564 * rgb[0] + 0.3575761 * rgb[1] + 0.1804375 * rgb[2],
              0.2126729 * rgb[0] + 0.7151522 * rgb[1] + 0.0721750 * rgb[2],
              0.0193339 * rgb[0] + 0.1191920 * rgb[1] + 0.9503041 * rgb[2]);
}

}  // namespace LTRE