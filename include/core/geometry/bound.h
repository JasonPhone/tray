/**
 * @file bound.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Class Bound3 and Bound2, for BVH, image subdivision, etc.
 * @version 0.1
 * @date 2023-03-14
 */
#pragma once
#include "core/tray.h"
#include "core/geometry/point.h"

/// @brief By 2023-03-14 we use AABB, which is defined on two corner points.

namespace TRay {
template <typename T>
class Bound3 {
 public:
  Bound3(const Point3<T> &p1, const Point3<T> &p2)
      : p_min(min(p1, p2)), p_max(max(p1, p2)) {}
  Bound3() {
    T maxv = std::numeric_limits<T>::max();
    T minv = std::numeric_limits<T>::lowest();
    p_min = Point3<T>(minv, minv, minv);
    p_max = Point3<T>(maxv, maxv, maxv);
  }
  Bound3(const Point3<T> &p) : Bound3(p, p) {}
  bool is_valid() const {
    return p_min.x <= p_max.x && p_min.y <= p_max.y && p_min.z <= p_max.z;
  }
  Point3<T> corner(int i) const {
    // The low 3-bit of i
    // Selected by 001, 010, 100
    return Point3<T>((i & 1) ? p_max.x : p_min.x, (i & 2) ? p_max.y : p_min.y,
                     (i & 4) ? p_max.z : p_min.z);
  }
  void expand(T delta) {
    Vector3<T> v(delta, delta, delta);
    p_min -= delta;
    p_max += delta;
  }
  Vector3<T> diagonal() const { return p_max - p_min; }
  T surface_area() const {
    Vector3<T> d = diagonal();
    return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
  }
  T volume() const {
    Vector3<T> d = diagonal();
    return d.x * d.y * d.z;
  }
  /// @brief Index of the axis with biggest extent.
  int max_extent() const { return max_dim(diagonal()); }
  /// @brief Lerp by the t given in a Point3f
  Point3<T> lerp(const Point3f &t) const {
    return Point3<T>(TRay::lerp(p_min.x, p_max.x, t.x),
                     TRay::lerp(p_min.y, p_max.y, t.y),
                     TRay::lerp(p_min.z, p_max.z, t.z));
  }
  Vector3<T> offset(const Point3<T> &p) const {
    Vector3<T> off = p - p_min;
    Vector3<T> d = diagonal();
    off.x /= d.x;
    off.y /= d.y;
    off.z /= d.z;
    return off;
  }
  Point3<T> p_min, p_max;
};
template <typename T>
class Bound2 {
 public:
  Bound2(const Point2<T> &p1, const Point2<T> &p2)
      : p_min(min(p1, p2)), p_max(max(p1, p2)) {}
  Bound2() {
    T maxv = std::numeric_limits<T>::max();
    T minv = std::numeric_limits<T>::lowest();
    p_min = Point2<T>(minv, minv);
    p_max = Point2<T>(maxv, maxv);
  }
  Bound2(const Point2<T> &p) : Bound2(p, p) {}
  bool is_valid() const { return p_min.x <= p_max.x && p_min.y <= p_max.y; }
  bool operator==(const Bound2<T> &other) const {
    return p_min == other.p_min && p_max == other.p_max;
  }
  Point2<T> corner(int i) const {
    // The low 2-bit of i
    // Selected by 01, 10
    return Point2<T>((i & 1) ? p_max.x : p_min.x, (i & 2) ? p_max.y : p_min.y);
  }
  void expand(T delta) {
    Vector2<T> v(delta, delta);
    p_min -= delta;
    p_max += delta;
  }
  Vector2<T> diagonal() const { return p_max - p_min; }
  T area() const {
    Vector2<T> d = diagonal();
    return d.x * d.y;
  }
  /// @brief Index of the axis with biggest extent.
  int max_extent() const { return max_dim(diagonal()); }
  /// @brief Lerp by the t given in a Point2f
  Point2<T> lerp(const Point2f &t) const {
    return Point2<T>(TRay::lerp(p_min.x, p_max.x, t.x),
                     TRay::lerp(p_min.y, p_max.y, t.y));
  }
  Vector2<T> offset(const Point2<T> &p) const {
    Vector2<T> off = p - p_min;
    Vector2<T> d = diagonal();
    off.x /= d.x;
    off.y /= d.y;
    return off;
  }
  Point2<T> p_min, p_max;
};
using Bound3i = Bound3<int>;
using Bound3f = Bound3<Float>;
using Bound2i = Bound2<int>;
using Bound2f = Bound2<Float>;

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
  Bound2iIterator &operator++() {
    increase();
    return *this;
  }
  /// @brief Suffix increase, itr++.
  Bound2iIterator operator++(int) {
    Bound2iIterator old = *this;
    ++*this;
    return old;
  }
  bool operator==(Bound2iIterator other) const {
    return pos.y == other.pos.y && bound == other.bound;
  }
  bool operator!=(Bound2iIterator other) const { return !(*this == other); }
  Point2i operator*() { return pos; }
  Bound2iIterator begin() { return Bound2iIterator(bound, bound.p_min); }
  Bound2iIterator end() { return Bound2iIterator(bound, bound.p_max); }

 private:
  void increase() {
    ++pos.x;
    if (pos.x == bound.p_max.x) {
      pos.x = bound.p_min.x;
      ++pos.y;
    }
  }

 private:
  Point2i pos;
  const Bound2i &bound;
};

// Bound3 inlines.
template <typename T>
inline Bound3<T> add(const Bound3<T> &b, const Point3<T> &p) {
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
inline Bound2<T> add(const Bound2<T> &b, const Point2<T> &p) {
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
}  // namespace TRay