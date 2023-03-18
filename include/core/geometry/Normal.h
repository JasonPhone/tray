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
  Float length() { return std::sqrt(length2()); }
  Normal3<T> operator+(const Normal3<T> &n) const;
  Normal3<T> &operator+=(const Normal3<T> &n);
  Normal3<T> operator-(const Normal3<T> &n) const;
  Normal3<T> operator-() const;
  Normal3<T> &operator-=(const Normal3<T> &n);
  Normal3<T> operator*(T s) const;
  friend Normal3<T> operator*(T s, const Normal3<T> &n);
  Normal3<T> &operator*=(T s);
  T operator[](int i) const;
  T &operator[](int i);
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
inline Normal3<T> normalize(const Normal3<T> &v) {
  return (1.0 / v.length()) * v;
}
template <typename T>
inline Normal3<T> align_with(const Normal3<T> &n, const Vector3<T> &v) {
  return (dot(n, v) < 0.0) ? -n : n;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Normal3<T> &n) {
  os << " <" << n.x << ", " << n.y << ", " << n.z << "> ";
  return os;
}
}  // namespace TRay