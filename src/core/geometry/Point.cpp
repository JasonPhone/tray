#include "core/geometry/Point.h"

namespace TRay {
// Point3 impl.
// ------------
template <typename T>
Point3<T> Point3<T>::operator+(const Vector3<T> &v) const {
  return Point3<T>(x + v.x, y + v.y, z + v.z);
}
template <typename T>
Point3<T> &Point3<T>::operator+=(const Vector3<T> &v) {
  x += v.x, y += v.y, z += v.z;
  return *this;
}
template <typename T>
Point3<T> Point3<T>::operator-(const Vector3<T> &v) const {
  return Point3<T>(x - v.x, y - v.y, z - v.z);
}
template <typename T>
Point3<T> &Point3<T>::operator-=(const Vector3<T> &v) {
  x -= v.x, y -= v.y, z -= v.z;
  return *this;
}
template <typename T>
Vector3<T> Point3<T>::operator-(const Point3<T> &p) const {
  return Vector3<T>(x - p.x, y - p.y, z - p.z);
}
template <typename T>
bool Point3<T>::operator==(const Point3<T> &p) const {
  return x == p.x && y == p.y && z == p.z;
}

/// @brief To compute weighted sum.
template <typename T>
Point3<T> Point3<T>::operator*(T s) const {
  return Point3<T>(x * s, y * s, z * s);
}

template <typename T>
Point3<T> operator*(Float s, Point3<T> p) {
  return p * s;
}
template <typename T>
Point3<T> &Point3<T>::operator*=(T s) {
  x *= s, y *= s, z *= s;
  return *this;
}
template <typename T>
Point3<T> Point3<T>::operator+(const Point3<T> &p) const {
  return Point3<T>(x + p.x, y + p.y, z + p.z);
}
template <typename T>
Point3<T> Point3<T>::operator+=(const Point3<T> &p) const {
  x += p.x, y += p.y, z += p.z;
  return *this;
}

template <typename T>
T Point3<T>::operator[](int i) const {
  ASSERT(i >= 0 && i <= 2);
  return (i == 0) ? x : (i == 1 ? y : z);
}
template <typename T>
T &Point3<T>::operator[](int i) {
  ASSERT(i >= 0 && i <= 2);
  return (i == 0) ? x : (i == 1 ? y : z);
}

// Point2 impl.
// ------------
template <typename T>
Point2<T> Point2<T>::operator+(const Vector2<T> &v) const {
  return Point2<T>(x + v.x, y + v.y);
}
template <typename T>
Point2<T> &Point2<T>::operator+=(const Vector2<T> &v) {
  x += v.x, y += v.y;
  return *this;
}
template <typename T>
Point2<T> Point2<T>::operator-(const Vector2<T> &v) const {
  return Point2<T>(x - v.x, y - v.y);
}
template <typename T>
Point2<T> &Point2<T>::operator-=(const Vector2<T> &v) {
  x -= v.x, y -= v.y;
  return *this;
}
template <typename T>
Vector2<T> Point2<T>::operator-(const Point2<T> &p) const {
  return Vector2<T>(x - p.x, y - p.y);
}
template <typename T>
bool Point2<T>::operator==(const Point2<T> &p) const {
  return x == p.x && y == p.y;
}

template <typename T>
Point2<T> Point2<T>::operator*(T s) const {
  return Point2<T>(x * s, y * s);
}
template <typename T>
Point2<T> operator*(Float s, Point2<T> p) {
  return p * s;
}
template <typename T>
Point2<T> &Point2<T>::operator*=(T s) {
  x *= s, y *= s;
  return *this;
}
template <typename T>
Point2<T> Point2<T>::operator+(const Point2<T> &p) const {
  return Point2<T>(x + p.x, y + p.y);
}
template <typename T>
Point2<T> Point2<T>::operator+=(const Point2<T> &p) const {
  x += p.x, y += p.y;
  return *this;
}
template <typename T>

T Point2<T>::operator[](int i) const {
  ASSERT(i == 0 || i == 1);
  return i == 0 ? x : y;
}
template <typename T>
T &Point2<T>::operator[](int i) {
  ASSERT(i == 0 || i == 1);
  return i == 0 ? x : y;
}
}  // namespace TRay