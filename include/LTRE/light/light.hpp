#ifndef _LTRE_LIGHT_H
#define _LTRE_LIGHT_H

namespace LTRE {

class Light {
 public:
  virtual Vec3 radiance(const IntersectInfo& info) const = 0;
};

}  // namespace LTRE

#endif