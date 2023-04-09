#pragma once
#include "core/TRay.h"
#include "core/stringformat.h"
#include "core/geometry/Vector.h"

namespace TRay {
// template <typename T>
// Point3<T> operator*(Float s, Point3<T> p);
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
  Vector3<T> operator-(const Point3<T> &p) const {
    return Vector3<T>(x - p.x, y - p.y, z - p.z);
  }
  bool operator==(const Point3<T> &p) const {
    return x == p.x && y == p.y && z == p.z;
  }
  /// @brief To compute weighted sum.
  Point3<T> operator*(T s) const { return Point3<T>(x * s, y * s, z * s); }
  friend Point3<T> operator*(T s, Point3<T> p) { return p * s; }
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
  std::string to_string() const {
    return string_format(" [%f, %f, %f] ", x, y, z);
  }

  T x, y, z;
};

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
  bool operator==(const Point2<T> &p) const { return x == p.x && y == p.y; }
  Point2<T> operator*(T s) const { return Point2<T>(x * s, y * s); }
  friend Point2<T> operator*(T s, Point2<T> p) { return p * s; }
  Point2<T> &operator*=(T s) {
    x *= s, y *= s;
    return *this;
  }
  Point2<T> operator+(const Point2<T> &p) const {
    return Point2<T>(x + p.x, y + p.y);
  }
  Point2<T> operator+=(const Point2<T> &p) {
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
  std::string to_string() const {
    return string_format(" [%f, %f] ", x, y);
  }

  T x, y;
};

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

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Point3<T> &p) {
  os << p.to_string();
  return os;
}
template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Point2<T> &p) {
  os << p.to_string();
  return os;
}
}  // namespace TRay