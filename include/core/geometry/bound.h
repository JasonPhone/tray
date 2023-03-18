/**
 * @file bound.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Class Bound3 and Bound2, for BVH, image subdivision, etc.
 * @version 0.1
 * @date 2023-03-14
 */
#pragma once
#include "core/TRay.h"
#include "core/geometry/Point.h"

/// @brief By 2023-03-14 we use AABB, which is defined on two corner points.

namespace TRay {
template <typename T>
class Bound3 {
 public:
  Bound3(const Point3<T> &p1, const Point3<T> &p2)
      : p_min(min(p1, p2)), p_max(max(p1, p2)) {}
  Bound3();
  Bound3(const Point3<T> &p) : Bound3(p, p) {}
  bool is_valid() const;
  Point3<T> corner(int i) const;
  void expand(T delta);
  Vector3<T> diagonal() const;
  T surface_area() const;
  T volume() const;
  /// @brief Index of the axis with biggest extent.
  int max_extent() const;
  /// @brief Lerp by the t given in a Point3f
  Point3<T> lerp(const Point3f &t) const;
  Vector3<T> offset(const Point3<T> &p) const;
  Point3<T> p_min, p_max;
};
template <typename T>
class Bound2 {
 public:
  Bound2(const Point2<T> &p1, const Point2<T> &p2)
      : p_min(min(p1, p2)), p_max(max(p1, p2)) {}
  Bound2();
  Bound2(const Point2<T> &p) : Bound2(p, p) {}
  bool is_valid() const;
  bool operator==(const Bound2<T> &other) const;
  Point2<T> corner(int i) const;
  void expand(T delta);
  Vector2<T> diagonal() const;
  T area() const;
  /// @brief Index of the axis with biggest extent.
  int max_extent() const;
  /// @brief Lerp by the t given in a Point2f
  Point2<T> lerp(const Point2f &t) const;
  Vector2<T> offset(const Point2<T> &p) const;
  Point2<T> p_min, p_max;
};

/**
 * @brief  Custom iterator for Bound2i.
 *         The range is [p_min, p_max), x goes first.
 * @note   Does not guarantee the behavior if change the bound while iterating
 * it.
 * @note   The std::iterator is deprecated since C++17.
 *         Code below derives from pbrt and
 *         https://stackoverflow.com/questions/37031805
 */
class Bound2iIterator {
 public:
  Bound2iIterator(const Bound2i &b, const Point2i &p) : bound(b), pos(p) {}
  Bound2iIterator(const Bound2i &b) : bound(b), pos(b.p_min) {}
  /// @brief Prefix increase, ++itr.
  Bound2iIterator &operator++();
  /// @brief Suffix increase, itr++.
  Bound2iIterator operator++(int);
  bool operator==(Bound2iIterator other) const;
  bool operator!=(Bound2iIterator other) const;
  Point2i operator*() const;
  Bound2iIterator begin() const;
  Bound2iIterator end() const;

 private:
  void increase();

  Point2i pos;
  const Bound2i &bound;
};

// Bound3 inlines.
template <typename T>
inline Bound3<T> bound_insert(const Bound3<T> &b, const Point3<T> &p) {
  return Bound3<T>(min(b.p_min, p), max(b.p_max, p));
}
template <typename T>
inline Bound3<T> bound_union(const Bound3<T> &b1, const Bound3<T> &b2) {
  return Bound3<T>(min(b1.p_min, b2.p_min), max(b1.p_max, b2.p_max));
}
/**
 * @brief Note that this will return an invalid(p_min < p_max) bound
 * when b1 and b2 does not overlap.
 */
template <typename T>
inline Bound3<T> bound_intersect(const Bound3<T> &b1, const Bound3<T> &b2) {
  Bound3<T> ret;
  ret.p_min = max(b1.p_min, b2.p_min);
  ret.p_max = min(b1.p_max, b2.p_max);
  return ret;
}
template <typename T>
inline bool bound_is_overlap(const Bound3<T> &b1, const Bound3<T> &b2) {
  bool x = (b1.p_max.x >= b2.p_min.x) && (b2.p_max.x >= b1.p_min.x);
  bool y = (b1.p_max.y >= b2.p_min.y) && (b2.p_max.y >= b1.p_min.y);
  bool z = (b1.p_max.z >= b2.p_min.z) && (b2.p_max.z >= b1.p_min.z);
  return x && y && z;
}
template <typename T>
inline bool point_in_bound_closed(const Point3<T> &p, const Bound3<T> &b) {
  bool x = (b.p_min.x <= p.x) && (p.x <= b.p_max.x);
  bool y = (b.p_min.y <= p.y) && (p.y <= b.p_max.y);
  bool z = (b.p_min.z <= p.z) && (p.z <= b.p_max.z);
  return x && y && z;
}
template <typename T>
inline bool point_in_bound_open(const Point3<T> &p, const Bound3<T> &b) {
  bool x = (b.p_min.x <= p.x) && (p.x < b.p_max.x);
  bool y = (b.p_min.y <= p.y) && (p.y < b.p_max.y);
  bool z = (b.p_min.z <= p.z) && (p.z < b.p_max.z);
  return x && y && z;
}

// Bound2 inlines.
template <typename T>
inline Bound2<T> bound_insert(const Bound2<T> &b, const Point2<T> &p) {
  return Bound2<T>(min(b.p_min, p), max(b.p_max, p));
}
template <typename T>
inline Bound2<T> bound_union(const Bound2<T> &b1, const Bound2<T> &b2) {
  return Bound2<T>(min(b1.p_min, b2.p_min), max(b1.p_max, b2.p_max));
}
/**
 * @brief Note that this will return an invalid(p_min < p_max) bound
 * when b1 and b2 does not overlap.
 */
template <typename T>
inline Bound2<T> bound_intersect(const Bound2<T> &b1, const Bound2<T> &b2) {
  Bound2<T> ret;
  ret.p_min = max(b1.p_min, b2.p_min);
  ret.p_max = min(b1.p_max, b2.p_max);
  return ret;
}
template <typename T>
inline bool bound_is_overlap(const Bound2<T> &b1, const Bound2<T> &b2) {
  bool x = (b1.p_max.x >= b2.p_min.x) && (b2.p_max.x >= b1.p_min.x);
  bool y = (b1.p_max.y >= b2.p_min.y) && (b2.p_max.y >= b1.p_min.y);
  return x && y;
}
template <typename T>
inline bool point_in_bound_closed(const Point2<T> &p, const Bound2<T> &b) {
  bool x = (b.p_min.x <= p.x) && (p.x <= b.p_max.x);
  bool y = (b.p_min.y <= p.y) && (p.y <= b.p_max.y);
  return x && y;
}
template <typename T>
inline bool point_in_bound_open(const Point2<T> &p, const Bound2<T> &b) {
  bool x = (b.p_min.x <= p.x) && (p.x < b.p_max.x);
  bool y = (b.p_min.y <= p.y) && (p.y < b.p_max.y);
  return x && y;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Bound3<T> &b) {
  os << " {" << b.p_min << ", " << b.p_max << "} ";
  return os;
}
template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Bound2<T> &b) {
  os << " {" << b.p_min << ", " << b.p_max << "} ";
  return os;
}
}  // namespace TRay