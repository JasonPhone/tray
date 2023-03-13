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

namespace TRay {
/// @brief Vector3 decl and impl.
/// --------------------
/**
 * \note Maybe we can make this using Swizzle?
 * \see https://zhuanlan.zhihu.com/p/340119757
 */
/**
 * @brief 3D vector.
 *
 * @tparam T Type of components.
 */
template <typename T>
class Vector3 {
 public:
  Vector3(T x, T y, T z) : x(x), y(y), z(z) { ASSERT(!has_NaN()); }
  Vector3() : Vector3(0, 0, 0) {}
  explicit Vector3(const Normal3<T> &n);
  bool has_NaN() const {
    return std::isnan(x) || std::isnan(y) || std::isnan(z);
  }
  Float length2() const { return x * x + y * y + z * z; }
  Float length() const { return std::sqrt(length2()); }
  Vector3<T> operator+(const Vector3<T> &v) const {
    return Vector3<T>(x + v.x, y + v.y, z + v.z);
  }
  Vector3<T> &operator+=(const Vector3<T> &v) {
    x += v.x, y += v.y, z += v.z;
    return *this;
  }
  Vector3<T> operator-(const Vector3<T> &v) const {
    return Vector3<T>(x - v.x, y - v.y, z - v.z);
  }
  Vector3<T> operator-() const { return Vector3<T>(-x, -y, -z); }
  Vector3<T> &operator-=(const Vector3<T> &v) {
    x -= v.x, y -= v.y, z -= v.z;
    return *this;
  }
  Vector3<T> operator*(T s) const { return Vector3<T>(x * s, y * s, z * s); }
  Vector3<T> &operator*=(T s) {
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
// Avoid circular definition.
template <typename T>
Vector3<T>::Vector3(const Normal3<T> &n) : Vector3(n.x, n.y, n.z) {}

/**
 * @brief 2D vector.
 *
 * @tparam T Type of x, y.
 */
template <typename T>
class Vector2 {
 public:
  Vector2(T x, T y) : x(x), y(y) { ASSERT(!has_NaN()); }
  Vector2() : Vector2(0, 0, 0) {}
  bool has_NaN() const { return std::isnan(x) || std::isnan(y); }
  Float length2() const { return x * x + y * y; }
  Float length() const { return std::sqrt(length2()); }
  Vector2<T> operator+(const Vector2<T> &v) const {
    return Vector2<T>(x + v.x, y + v.y);
  }
  Vector2<T> &operator+=(const Vector2<T> &v) {
    x += v.x, y += v.y;
    return *this;
  }
  Vector2<T> operator-(const Vector2<T> &v) const {
    return Vector2<T>(x - v.x, y - v.y);
  }
  Vector2<T> operator-() const { return Vector2<T>(-x, -y); }
  Vector2<T> &operator-=(const Vector2<T> &v) {
    x -= v.x, y -= v.y;
    return *this;
  }
  Vector2<T> operator*(T s) const { return Vector2<T>(x * s, y * s); }
  Vector2<T> &operator*=(T s) {
    x *= s, y *= s;
    return *this;
  }
  T operator[](int i) const {
    ASSERT(i == 0 || i == 1);
    return i == 0 ? x : y;
  }
  T &operator[](int i) {
    ASSERT(i == 0 || i == 1);
    return i == 0 ? x : y;
  }
  T x, y;
};

/**
 * @brief 3D point.
 *
 * @tparam T
 */
template <typename T>
class Point3 {
 public:
  Point3(T x, T y, T z) : x(x), y(y), z(z) { ASSERT(!has_NaN()); }
  Point3() : Point3(0, 0, 0) {}
  /// @brief Component type explicit conversion. Changing the component type.
  /// @tparam U Original type.
  template <typename U>
  explicit Point3(const Point3<U> &p) : Point3(T(p.x), T(p.y), T(p.z)) {}
  /// @brief Convert out to vector of some type.
  /// Example: Vector3i v = Vector3i(p)
  template <typename U>
  explicit operator Vector3<U>() const {
    return Vector3<U>(x, y, z);
  }
  bool has_NaN() const {
    return std::isnan(x) || std::isnan(y) || std::isnan(z);
  }

  /// @note In the order of "Point + Vector"
  Point3<T> operator+(const Vector3<T> &v) const {
    return Point3<T>(x + v.x, y + v.y, z + v.z);
  }
  Point3<T> &operator+=(const Vector3<T> &v) {
    x += v.x, y += v.y, z += v.z;
    return *this;
  }
  Point3<T> operator-(const Vector3<T> &v) const {
    return Point3<T>(x - v.x, y - v.y, z - v.z);
  }
  Point3<T> &operator-=(const Vector3<T> &v) {
    x -= v.x, y -= v.y, z -= v.z;
    return *this;
  }
  Vector3<T> operator-(const Point3<T> &v) const {
    return Vector3<T>(x - v.x, y - v.y, z - v.z);
  }

  /// @brief To compute weighted sum.
  Point3<T> operator*(T s) const { return Point3<T>(x * s, y * s, z * s); }
  friend Point3<T> operator*(Float s, Point3<T> p) { return p * s; }
  Point3<T> &operator*=(T s) {
    x *= s, y *= s, z *= s;
    return *this;
  }
  Point3<T> operator+(const Point3<T> &p) const {
    return Point3<T>(x + p.x, y + p.y, z + p.z);
  }
  Point3<T> operator+=(const Point3<T> &p) const {
    x += p.x, y += p.y, z += p.z;
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

/**
 * @brief 2D point.
 *
 * @tparam T
 */
template <typename T>
class Point2 {
 public:
  Point2(T x, T y) : x(x), y(y) { ASSERT(!has_NaN()); }
  Point2() : Point2(0, 0) {}
  /// @brief A conversion dropping z component.
  explicit Point2(const Point3<T> &p) : Point2(p.x, p.y) {}
  /// @brief Component type explicit conversion.
  /// @tparam U Original type.
  template <typename U>
  explicit Point2(const Point2<U> &p) : Point2(T(p.x), T(p.y)) {}
  /// @brief Example: Vector2i v = Vector2i(p)
  template <typename U>
  explicit operator Vector2<U>() const {
    return Vector2<U>(x, y);
  }
  bool has_NaN() const { return std::isnan(x) || std::isnan(y); }

  Point2<T> operator+(const Vector2<T> &v) const {
    return Point2<T>(x + v.x, y + v.y);
  }
  Point2<T> &operator+=(const Vector2<T> &v) {
    x += v.x, y += v.y;
    return *this;
  }
  Point2<T> operator-(const Vector2<T> &v) const {
    return Point2<T>(x - v.x, y - v.y);
  }
  Point2<T> &operator-=(const Vector2<T> &v) {
    x -= v.x, y -= v.y;
    return *this;
  }
  Vector2<T> operator-(const Point2<T> &p) const {
    return Vector2<T>(x - p.x, y - p.y);
  }

  Point2<T> operator*(T s) const { return Point2<T>(x * s, y * s); }
  friend Point2<T> operator*(Float s, Point2<T> p) { return p * s; }
  Point2<T> &operator*=(T s) {
    x *= s, y *= s;
    return *this;
  }
  Point2<T> operator+(const Point2<T> &p) const {
    return Point2<T>(x + p.x, y + p.y);
  }
  Point2<T> operator+=(const Point2<T> &p) const {
    x += p.x, y += p.y;
    return *this;
  }

  T operator[](int i) const {
    ASSERT(i == 0 || i == 1);
    return i == 0 ? x : y;
  }
  T &operator[](int i) {
    ASSERT(i == 0 || i == 1);
    return i == 0 ? x : y;
  }
  T x, y;
};

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

// For convenience
using Vector3f = Vector3<Float>;
using Vector3i = Vector3<int>;
using Vector2f = Vector2<Float>;
using Vector2i = Vector2<int>;
using Point3i = Point3<int>;
using Point3f = Point3<Float>;
using Point2i = Point2<int>;
using Point2f = Point2<Float>;
using Normal3f = Normal3<Float>;

/// @brief Vector3 inlines.
template <typename T>
inline Vector3<T> abs(const Vector3<T> &v) {
  return Vector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}
template <typename T>
inline T dot(const Vector3<T> &u, const Vector3<T> &v) {
  return u.x * v.x + u.y * v.y + u.z * v.z;
}
template <typename T>
inline T abs_dot(const Vector3<T> &u, const Vector3<T> &v) {
  return std::abs(dot(u, v));
}
template <typename T>
inline Vector3<T> cross(const Vector3<T> &u, const Vector3<T> &v) {
  // In case of floating prescision error
  double ux = u.x, uy = u.y, uz = u.z;
  double vx = v.x, vy = v.y, vz = v.z;
  return Vector3<T>(uy * vz - uz * vy, uz * vx - ux * vz, ux * vy - uy * vx);
}
template <typename T>
inline Vector3<T> normalize(const Vector3<T> &v) {
  return (1.0 / v.length()) * v;
}
template <typename T>
inline Vector3<T> min_component(const Vector3<T> &v) {
  return std::min(v.x, std::min(v.y, v.z));
}
template <typename T>
inline Vector3<T> max_component(const Vector3<T> &v) {
  return std::max(v.x, std::max(v.y, v.z));
}
/// @brief Get (one of) the index of dimension which has the maximum component.
template <typename T>
inline int max_dim(const Vector3<T> &v) {
  return (v.x > v.y) ? (v.x > v.z ? 0 : 2) : (v.y > v.z ? 1 : 2);
}
/// @brief Get the vector with every component being
/// the bigger one of the params.
template <typename T>
inline Vector3<T> max(const Vector3<T> &u, const Vector3<T> &v) {
  return Vector3<T>(std::max(u.x, v.x), std::max(u.y, v.y), std::max(u.z, v.z));
}
/// @brief Get the vector with every component being the smaller one of the
/// params.
template <typename T>
inline Vector3<T> min(const Vector3<T> &u, const Vector3<T> &v) {
  return Vector3<T>(std::min(u.x, v.x), std::min(u.y, v.y), std::min(u.z, v.z));
}
/// @brief Permute the components by given indices.
template <typename T>
inline Vector3<T> permute(const Vector3<T> &v, int x, int y, int z) {
  return Vector3<T>(v[x], v[y], v[z]);
}
/**
 * @brief Create a local coordinate system.
 *
 * @param u Main basis. Supposed to be NORMALIZED.
 * @param v One of the basis.
 * @param w One of the basis.
 */
template <typename T>
inline void MakeCoordSystem(const Vector3<T> &u, Vector3<T> *v, Vector3<T> *w) {
  ASSERT(v != nullptr && w != nullptr);
  // Make v perpendicular to u by removing a smaller component.
  if (std::abs(u.x) > std::abs(u.y)) {
    *v = Vector3<T>(u.z, 0, -u.x) * (1.0 / std::sqrt(u.x * u.x + u.z * u.z));
  } else {
    *v = Vector3<T>(0, u.z, -u.y) * (1.0 / std::sqrt(u.y * u.y + u.z * u.z));
  }
  *w = cross(u, *v);
}

/// @brief Vector2 inlines.
template <typename T>
inline Vector2<T> abs(const Vector2<T> &v) {
  return Vector2<T>(std::abs(v.x), std::abs(v.y));
}
template <typename T>
inline T dot(const Vector2<T> &u, const Vector2<T> &v) {
  return u.x * v.x + u.y * v.y;
}
template <typename T>
inline T abs_dot(const Vector2<T> &u, const Vector2<T> &v) {
  return std::abs(dot(u, v));
}
template <typename T>
inline T cross(const Vector2<T> &u, const Vector2<T> &v) {
  // In case of floating prescision error
  double ux = u.x, uy = u.y;
  double vx = v.x, vy = v.y;
  return ux * vy - vx * uy;
}
template <typename T>
inline Vector2<T> normalize(const Vector2<T> &v) {
  return (1.0 / v.length()) * v;
}
template <typename T>
inline Vector2<T> min_component(const Vector2<T> &v) {
  return std::min(v.x, v.y);
}
template <typename T>
inline Vector2<T> max_component(const Vector2<T> &v) {
  return std::max(v.x, v.y);
}
/// @brief Get (one of) the index of dimension which has the maximum component.
template <typename T>
inline int max_dim(const Vector2<T> &v) {
  return (v.x > v.y) ? 0 : 1;
}
/// @brief Get the vector with every component being
/// the bigger one of the params.
template <typename T>
inline Vector2<T> max(const Vector2<T> &u, const Vector2<T> &v) {
  return Vector2<T>(std::max(u.x, v.x), std::max(u.y, v.y));
}
/// @brief Get the vector with every component being
/// the smaller one of the params.
template <typename T>
inline Vector2<T> min(const Vector2<T> &u, const Vector2<T> &v) {
  return Vector2<T>(std::min(u.x, v.x), std::min(u.y, v.y));
}
/// @brief Permute the components by given indices.
template <typename T>
inline Vector2<T> permute(const Vector2<T> &v, int x, int y) {
  return Vector2<T>(v[x], v[y]);
}

/// @brief Point3 inlines.
template <typename T>
inline Float distance(const Point3<T> &p1, const Point3<T> &p2) {
  return (p1 - p2).length();
}
template <typename T>
inline Float distance2(const Point3<T> &p1, const Point3<T> &p2) {
  return (p1 - p2).length2();
}
template <typename T>
inline Point3<T> lerp(const Point3<T> &p1, const Point3<T> &p2, Float t) {
  return p1 * (1 - t) + p2 * t;
}
template <typename T>
inline Point3<T> max(const Point3<T> &p1, const Point3<T> &p2) {
  return Point3<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
                   std::max(p1.z, p2.z));
}
template <typename T>
inline Point3<T> min(const Point3<T> &p1, const Point3<T> &p2) {
  return Point3<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
                   std::min(p1.z, p2.z));
}
template <typename T>
inline Point3<T> floor(const Point3<T> &p) {
  return Point3<T>(std::floor(p.x), std::floor(p.y), std::floor(p.z));
}
template <typename T>
inline Point3<T> ceil(const Point3<T> &p) {
  return Point3<T>(std::ceil(p.x), std::ceil(p.y), std::ceil(p.z));
}
template <typename T>
inline Point3<T> permute(const Point3<T> &p, int x, int y, int z) {
  return Point3<T>(p[x], p[y], p[z]);
}

/// @brief Point2 inlines.
template <typename T>
inline Float distance(const Point2<T> &p1, const Point2<T> &p2) {
  return (p1 - p2).length();
}
template <typename T>
inline Float distance2(const Point2<T> &p1, const Point2<T> &p2) {
  return (p1 - p2).length2();
}
template <typename T>
inline Point2<T> lerp(const Point2<T> &p1, const Point2<T> &p2, Float t) {
  return p1 * (1 - t) + p2 * t;
}
template <typename T>
inline Point2<T> max(const Point2<T> &p1, const Point2<T> &p2) {
  return Point2<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
}
template <typename T>
inline Point2<T> min(const Point2<T> &p1, const Point2<T> &p2) {
  return Point2<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
}
template <typename T>
inline Point2<T> floor(const Point2<T> &p) {
  return Point2<T>(std::floor(p.x), std::floor(p.y));
}
template <typename T>
inline Point2<T> ceil(const Point2<T> &p) {
  return Point2<T>(std::ceil(p.x), std::ceil(p.y));
}
template <typename T>
inline Point2<T> permute(const Point2<T> &p, int x, int y) {
  return Point2<T>(p[x], p[y]);
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

/// @brief Normal3 inlines.
// template <typename T>
// inline Normal3<T> abs(const Vector3<T> &v) {
//   return Normal3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
// }
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
}  // namespace TRay