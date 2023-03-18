#pragma once
#include "core/TRay.h"
#include "core/geometry/Vector.h"

namespace TRay {
class Quaternion {
 public:
  Quaternion() : imag(0, 0, 0), real(1) {}
  Quaternion(const Transform &t);
  Quaternion &operator+=(const Quaternion &other);
  Quaternion operator+(const Quaternion &other) const;
  Quaternion &operator-=(const Quaternion &other);
  Quaternion operator-() const;
  Quaternion operator-(const Quaternion &other) const;
  Quaternion &operator*=(Float s);
  Quaternion operator*(Float s) const;
  friend Quaternion operator*(Float s, const Quaternion &q);
  Transform to_transform() const;

  Vector3f imag;
  Float real;
};
Float dot(const Quaternion &q1, const Quaternion &q2);
Quaternion normalize(const Quaternion &q);
Quaternion sph_lerp(const Quaternion &q1, const Quaternion &q2, Float t);
inline std::ostream &operator<<(std::ostream &os, const Quaternion &q) {
  os << " [" << q.imag.x << ", " << q.imag.y << ", " << q.imag.z << ", "
     << q.real << "] ";
  return os;
}
}  // namespace TRay