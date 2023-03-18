/**
 * @file Vector.cpp
 * @author ja50n (zs_feng@qq.com)
 * @brief Vector impl.
 * @version 0.1
 * @date 2023-03-18
 */
#include "core/geometry/Vector.h"

namespace TRay {
/**
 * \note Maybe we can make this using Swizzle?
 * \see https://zhuanlan.zhihu.com/p/340119757
 */
template <typename T>
Vector3<T>::Vector3(const Normal3<T> &n) : Vector3(n.x, n.y, n.z) {}
template <typename T>
Vector3<T> Vector3<T>::operator+(const Vector3<T> &v) const {
  return Vector3<T>(x + v.x, y + v.y, z + v.z);
}
template <typename T>
Vector3<T> &Vector3<T>::operator+=(const Vector3<T> &v) {
  x += v.x, y += v.y, z += v.z;
  return *this;
}
template <typename T>
Vector3<T> Vector3<T>::operator-(const Vector3<T> &v) const {
  return Vector3<T>(x - v.x, y - v.y, z - v.z);
}
template <typename T>
Vector3<T> Vector3<T>::operator-() const {
  return Vector3<T>(-x, -y, -z);
}
template <typename T>
Vector3<T> &Vector3<T>::operator-=(const Vector3<T> &v) {
  x -= v.x, y -= v.y, z -= v.z;
  return *this;
}
template <typename T>
Vector3<T> Vector3<T>::operator*(T s) const {
  return Vector3<T>(x * s, y * s, z * s);
}
template <typename T>
Vector3<T> &Vector3<T>::operator*=(T s) {
  x *= s, y *= s, z *= s;
  return *this;
}
template <typename T>
Vector3<T> operator*(T s, const Vector3<T> &v) {
  return v * s;
}
template <typename T>
T Vector3<T>::operator[](int i) const {
  ASSERT(i >= 0 && i <= 2);
  return (i == 0) ? x : (i == 1 ? y : z);
}
template <typename T>
T &Vector3<T>::operator[](int i) {
  ASSERT(i >= 0 && i <= 2);
  return (i == 0) ? x : (i == 1 ? y : z);
}

template <typename T>
Vector2<T> Vector2<T>::operator+(const Vector2<T> &v) const {
  return Vector2<T>(x + v.x, y + v.y);
}
template <typename T>
Vector2<T> &Vector2<T>::operator+=(const Vector2<T> &v) {
  x += v.x, y += v.y;
  return *this;
}
template <typename T>
Vector2<T> Vector2<T>::operator-(const Vector2<T> &v) const {
  return Vector2<T>(x - v.x, y - v.y);
}
template <typename T>
Vector2<T> Vector2<T>::operator-() const {
  return Vector2<T>(-x, -y);
}
template <typename T>
Vector2<T> &Vector2<T>::operator-=(const Vector2<T> &v) {
  x -= v.x, y -= v.y;
  return *this;
}
template <typename T>
Vector2<T> Vector2<T>::operator*(T s) const {
  return Vector2<T>(x * s, y * s);
}
template <typename T>
Vector2<T> operator*(T s, const Vector2<T> &v) {
  return v * s;
}
template <typename T>
Vector2<T> &Vector2<T>::operator*=(T s) {
  x *= s, y *= s;
  return *this;
}
template <typename T>
T Vector2<T>::operator[](int i) const {
  ASSERT(i == 0 || i == 1);
  return i == 0 ? x : y;
}
template <typename T>
T &Vector2<T>::operator[](int i) {
  ASSERT(i == 0 || i == 1);
  return i == 0 ? x : y;
}

}  // namespace TRay