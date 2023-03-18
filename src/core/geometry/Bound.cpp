#include "core/geometry/Bound.h"
#include "core/geometry/Point.h"
namespace TRay {
// Bound3 impl.
// ------------
template <typename T>
Bound3<T>::Bound3() {
  T maxv = std::numeric_limits<T>::max();
  T minv = std::numeric_limits<T>::lowest();
  p_min = Point3<T>(minv, minv, minv);
  p_max = Point3<T>(maxv, maxv, maxv);
}
template <typename T>
bool Bound3<T>::is_valid() const {
  return p_min.x <= p_max.x && p_min.y <= p_max.y && p_min.z <= p_max.z;
}
template <typename T>
Point3<T> Bound3<T>::corner(int i) const {
  // The low 3-bit of i
  // Selected by 001, 010, 100
  return Point3<T>((i & 1) ? p_max.x : p_min.x, (i & 2) ? p_max.y : p_min.y,
                   (i & 4) ? p_max.z : p_min.z);
}
template <typename T>
void Bound3<T>::expand(T delta) {
  Vector3<T> v(delta, delta, delta);
  p_min -= delta;
  p_max += delta;
}
template <typename T>
Vector3<T> Bound3<T>::diagonal() const {
  return p_max - p_min;
}
template <typename T>
T Bound3<T>::surface_area() const {
  Vector3<T> d = diagonal();
  return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
}
template <typename T>
T Bound3<T>::volume() const {
  Vector3<T> d = diagonal();
  return d.x * d.y * d.z;
}
/// @brief Index of the axis with biggest extent.
template <typename T>
int Bound3<T>::max_extent() const {
  return max_dim(diagonal());
}
/// @brief Lerp by the t given in a Point3f
template <typename T>
Point3<T> Bound3<T>::lerp(const Point3f &t) const {
  return Point3<T>(TRay::lerp(p_min.x, p_max.x, t.x),
                   TRay::lerp(p_min.y, p_max.y, t.y),
                   TRay::lerp(p_min.z, p_max.z, t.z));
}
template <typename T>
Vector3<T> Bound3<T>::offset(const Point3<T> &p) const {
  Vector3<T> off = p - p_min;
  Vector3<T> d = diagonal();
  off.x /= d.x;
  off.y /= d.y;
  off.z /= d.z;
  return off;
}

// Bound2 impl.
// ------------
template <typename T>
Bound2<T>::Bound2() {
  T maxv = std::numeric_limits<T>::max();
  T minv = std::numeric_limits<T>::lowest();
  p_min = Point2<T>(minv, minv);
  p_max = Point2<T>(maxv, maxv);
}
template <typename T>
bool Bound2<T>::is_valid() const {
  return p_min.x <= p_max.x && p_min.y <= p_max.y;
}
template <typename T>
bool Bound2<T>::operator==(const Bound2<T> &other) const {
  return p_min == other.p_min && p_max == other.p_max;
}
template <typename T>
Point2<T> Bound2<T>::corner(int i) const {
  // The low 2-bit of i
  // Selected by 01, 10
  return Point2<T>((i & 1) ? p_max.x : p_min.x, (i & 2) ? p_max.y : p_min.y);
}
template <typename T>
void Bound2<T>::expand(T delta) {
  Vector2<T> v(delta, delta);
  p_min -= delta;
  p_max += delta;
}
template <typename T>
Vector2<T> Bound2<T>::diagonal() const {
  return p_max - p_min;
}
template <typename T>
T Bound2<T>::area() const {
  Vector2<T> d = diagonal();
  return d.x * d.y;
}
/// @brief Index of the axis with biggest extent.
template <typename T>
int Bound2<T>::max_extent() const {
  return max_dim(diagonal());
}
/// @brief Lerp by the t given in a Point2f
template <typename T>
Point2<T> Bound2<T>::lerp(const Point2f &t) const {
  return Point2<T>(TRay::lerp(p_min.x, p_max.x, t.x),
                   TRay::lerp(p_min.y, p_max.y, t.y));
}
template <typename T>
Vector2<T> Bound2<T>::offset(const Point2<T> &p) const {
  Vector2<T> off = p - p_min;
  Vector2<T> d = diagonal();
  off.x /= d.x;
  off.y /= d.y;
  return off;
}

// Bound2iIterator impl.
// ---------------------
Bound2iIterator &Bound2iIterator::operator++() {
  increase();
  return *this;
}
/// @brief Suffix increase, itr++.
Bound2iIterator Bound2iIterator::operator++(int) {
  Bound2iIterator old = *this;
  ++*this;
  return old;
}
bool Bound2iIterator::operator==(Bound2iIterator other) const {
  return pos.y == other.pos.y && bound == other.bound;
}
bool Bound2iIterator::operator!=(Bound2iIterator other) const {
  return !(*this == other);
}
Point2i Bound2iIterator::operator*() const{ return pos; }
Bound2iIterator Bound2iIterator::begin()const {
  return Bound2iIterator(bound, bound.p_min);
}
Bound2iIterator Bound2iIterator::end() const {
  return Bound2iIterator(bound, bound.p_max);
}

void Bound2iIterator::increase() {
  ++pos.x;
  if (pos.x == bound.p_max.x) {
    pos.x = bound.p_min.x;
    ++pos.y;
  }
}

}  // namespace TRay