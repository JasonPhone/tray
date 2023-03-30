#pragma once
#include "core/TRay.h"
#include "core/geometry/Vector.h"
namespace TRay {
template <typename T>
Normal3<T> operator*(T s, const Normal3<T> &n);
/// @brief 3D normal decl.
/// ----------------------
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
  Float length() const { return std::sqrt(length2()); }
  bool operator==(const Normal3<T> &other) const {
    return x == other.x && y == other.y && z == other.z;
  }
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
inline Vector3<T> cross(const Vector3<T> &u, const Normal3<T> &v) {
  // In case of floating prescision error
  double ux = u.x, uy = u.y, uz = u.z;
  double vx = v.x, vy = v.y, vz = v.z;
  return Vector3<T>(uy * vz - uz * vy, uz * vx - ux * vz, ux * vy - uy * vx);
}
template <typename T>
inline Vector3<T> cross(const Normal3<T> &u, const Vector3<T> &v) {
  // In case of floating prescision error
  double ux = u.x, uy = u.y, uz = u.z;
  double vx = v.x, vy = v.y, vz = v.z;
  return Vector3<T>(uy * vz - uz * vy, uz * vx - ux * vz, ux * vy - uy * vx);
}
template <typename T>
inline Normal3<T> normalize(const Normal3<T> &v) {
  return (1.0 / v.length()) * v;
}
/**
 * @brief Return the (maybe flipped) normal whose direction is aligned with
 * given vector.
 *
 * @param n Normal to flip.
 * @param v Vector to align with.
 */
template <typename T>
inline Normal3<T> align_with(const Normal3<T> &n, const Vector3<T> &v) {
  return (dot(n, v) < 0.0) ? -n : n;
}
template <typename T>
inline Normal3<T> align_with(const Normal3<T> &n, const Normal3<T> &v) {
  return (dot(n, v) < 0.0) ? -n : n;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Normal3<T> &n) {
  os << " <" << n.x << ", " << n.y << ", " << n.z << "> ";
  return os;
}
}  // namespace TRay