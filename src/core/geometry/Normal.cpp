#include "core/geometry/Normal.h"

namespace TRay {
// Normal3 impl.
// -------------

template <typename T>
Normal3<T> Normal3<T>::operator+(const Normal3<T> &n) const {
  return Normal3<T>(x + n.x, y + n.y, z + n.z);
}
template <typename T>
Normal3<T> &Normal3<T>::operator+=(const Normal3<T> &n) {
  x += n.x, y += n.y, z += n.z;
  return *this;
}
template <typename T>
Normal3<T> Normal3<T>::operator-(const Normal3<T> &n) const {
  return Normal3<T>(x - n.x, y - n.y, z - n.z);
}
template <typename T>
Normal3<T> Normal3<T>::operator-() const {
  return Normal3<T>(-x, -y, -z);
}
template <typename T>
Normal3<T> &Normal3<T>::operator-=(const Normal3<T> &n) {
  x -= n.x, y -= n.y, z -= n.z;
  return *this;
}
template <typename T>
Normal3<T> Normal3<T>::operator*(T s) const {
  return Normal3<T>(x * s, y * s, z * s);
}
template <typename T>
Normal3<T> operator*(T s, const Normal3<T> &n) {
  return n * s;
}
template <typename T>
Normal3<T> &Normal3<T>::operator*=(T s) {
  x *= s, y *= s, z *= s;
  return *this;
}
template <typename T>
T Normal3<T>::operator[](int i) const {
  ASSERT(i >= 0 && i <= 2);
  return (i == 0) ? x : (i == 1 ? y : z);
}
template <typename T>
T &Normal3<T>::operator[](int i) {
  ASSERT(i >= 0 && i <= 2);
  return (i == 0) ? x : (i == 1 ? y : z);
}

}  // namespace TRay