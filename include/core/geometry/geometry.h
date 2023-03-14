/**
 * @file geometry.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Geometry and maths basis for TRay.
 * @version 0.1
 * @date 2023-03-11
 */

/**
 * In 3D spaces, the origin and three basis vectors are called "frame".
 * They define a coordinate system.
 *
 * Vectors express the direction and distance,
 * while points need the origin to express the position.
 * So vector and points are different, in spite of the
 * same presentation.
 *
 * Coordinate frame and points, vectors rely on each other
 * to be meaningful, so we specify a standard "world space" frame
 * which has (0, 0, 0) as origin and (1, 0, 0), (0, 1, 0) and (0, 0, 1)
 * as basis vectors.
 * All other frames are relative to world space.
 *
 * The code impl could be optimized using a 3-tuple and inheritance
 * to reduce the similar computation. But we want to make the difference clear.
 */

// ------------------
//   Handness: left
// ------------------
#pragma once
#include "core/tray.h"
#include "core/geometry/vector.h"
#include "core/geometry/point.h"

namespace TRay {
/**
 * @brief 3D normal.
 *
 * @tparam T Type of components.
 */
template <typename T>
class Normal3 {
 public:
  Normal3(T x, T y, T z) : x(x), y(y), z(z) { ASSERT(!has_NaN()); }
  Normal3() : Normal3(0, 0, 0) {}
  explicit Normal3(const Vector3<T> &v) : Normal3(v.x, v.y, v.z) {}
  bool has_NaN() const {
    return std::isnan(x) || std::isnan(y) || std::isnan(z);
  }
  Float length2() const { return x * x + y * y * z * z; }
  Float length() { return std::sqrt(length2()); }
  Normal3<T> operator+(const Normal3<T> &n) const {
    return Normal3<T>(x + n.x, y + n.y, z + n.z);
  }
  Normal3<T> &operator+=(const Normal3<T> &n) {
    x += n.x, y += n.y, z += n.z;
    return *this;
  }
  Normal3<T> operator-(const Normal3<T> &n) const {
    return Normal3<T>(x - n.x, y - n.y, z - n.z);
  }
  Normal3<T> operator-() const { return Normal3<T>(-x, -y, -z); }
  Normal3<T> &operator-=(const Normal3<T> &n) {
    x -= n.x, y -= n.y, z -= n.z;
    return *this;
  }
  Normal3<T> operator*(T s) const { return Normal3<T>(x * s, y * s, z * s); }
  friend Normal3<T> operator*(T s, const Normal3<T> &n) { return n * s; }
  Normal3<T> &operator*=(T s) {
    x *= s, y *= s, z *= s;
    return *this;
  }
  T operator[](int i) const {
    ASSERT(i >= 0 && i <= 2);
    return (i == 0) ? x : (i == 1 ? y : z);
  }
  T &operator[](int i) {
    ASSERT(i >= 0 && i <= 2);
    return (i == 0) ? x : (i == 1 ? y : z);
  }
  T x, y, z;
};

using Normal3f = Normal3<Float>;

/// @brief Normal3 inlines.
template <typename T>
inline T dot(const Normal3<T> &u, const Normal3<T> &v) {
  return u.x * v.x + u.y * v.y + u.z * v.z;
}
template <typename T>
inline T dot(const Normal3<T> &u, const Vector3<T> &v) {
  return u.x * v.x + u.y * v.y + u.z * v.z;
}
template <typename T>
inline T dot(const Vector3<T> &u, const Normal3<T> &v) {
  return u.x * v.x + u.y * v.y + u.z * v.z;
}
template <typename T>
inline T abs_dot(const Normal3<T> &u, const Normal3<T> &v) {
  return std::abs(dot(u, v));
}
template <typename T>
inline T abs_dot(const Normal3<T> &u, const Vector3<T> &v) {
  return std::abs(dot(u, v));
}
template <typename T>
inline T abs_dot(const Vector3<T> &u, const Normal3<T> &v) {
  return std::abs(dot(u, v));
}
template <typename T>
inline Normal3<T> normalize(const Normal3<T> &v) {
  return (1.0 / v.length()) * v;
}
template <typename T>
inline Normal3<T> align_with(const Normal3<T> &n, const Vector3<T> &v) {
  return (dot(n, v) < 0.0) ? -n : n;
}

// Verbose
template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Vector3<T> &v) {
  os << " (" << v.x << ", " << v.y << ", " << v.z << ") ";
  return os;
}
template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Vector2<T> &v) {
  os << " (" << v.x << ", " << v.y << ") ";
  return os;
}
template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Point3<T> &p) {
  os << " [" << p.x << ", " << p.y << ", " << p.z << "] ";
  return os;
}
template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Point2<T> &p) {
  os << " [" << p.x << ", " << p.y << "] ";
  return os;
}
template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Normal3<T> &n) {
  os << " <" << n.x << ", " << n.y << ", " << n.z << "> ";
  return os;
}
}  // namespace TRay