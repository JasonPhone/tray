#pragma once
#include "core/TRay.h"
#include "core/stringformat.h"

namespace TRay {
/// \note Maybe we can make this using Swizzle?
/// \see https://zhuanlan.zhihu.com/p/340119757

template <typename T>
class Vector3 {
 public:
  Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) { ASSERT(!has_NaN()); }
  Vector3() : Vector3(0, 0, 0) {}
  explicit Vector3(const Normal3<T> &n) : Vector3(n.x, n.y, n.z) {}
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
  bool operator==(const Vector3<T> v) const {
    return z == v.z && y == v.y && z == v.z;
  }
  bool operator!=(const Vector3<T> v) const { return !(*this == v); }
  /**
   * @brief The friend template overloading:
   * https://stackoverflow.com/questions/4660123/
   */
  friend Vector3<T> operator*(T s, const Vector3<T> &v) { return v * s; }
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
  std::string to_string() const {
    return string_format("[%f, %f, %f]", x * 1.0, y * 1.0, z * 1.0);
  }

  T x, y, z;
};

// template <typename T>
// Vector2<T> operator*(T s, const Vector2<T> &v);
template <typename T>
class Vector2 {
 public:
  Vector2(T _x, T _y) : x(_x), y(_y) { ASSERT(!has_NaN()); }
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
  friend Vector2<T> operator*(T s, const Vector2<T> &v) { return v * s; }
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
inline T min_component(const Vector3<T> &v) {
  return std::min(v.x, std::min(v.y, v.z));
}
template <typename T>
inline T max_component(const Vector3<T> &v) {
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
/// @brief Get the unit direction vector by spherical angles.
inline Vector3f spherical_direction(Float sin_theta, Float cos_theta,
                                    Float phi) {
  return Vector3f(sin_theta * std::cos(phi), sin_theta * std::sin(phi),
                  cos_theta);
}
/// @brief Get the unit direction vector by spherical angles in given frame.
inline Vector3f spherical_direction(Float sin_theta, Float cos_theta, Float phi,
                                    const Vector3f &x, const Vector3f &y,
                                    const Vector3f &z) {
  return sin_theta * std::cos(phi) * x + sin_theta * std::sin(phi) * y +
         cos_theta * z;
}
inline Float spherical_theta_of(const Vector3f &v) {
  return std::acos(clamp(v.z, -1, 1));
}
inline Float spherical_phi_of(const Vector3f &v) {
  Float p = std::atan2(v.y, v.x);
  return (p < 0) ? (p + 2 * PI) : p;
}

/**
 * @brief Create a local coordinate system.
 *        w = cross(u, v), left-handed.
 * @param u Main basis. Supposed to be NORMALIZED.
 * @param v One of the basis.
 * @param w One of the basis.
 */
template <typename T>
inline void make_coord_system(const Vector3<T> &u, Vector3<T> *v,
                              Vector3<T> *w) {
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
inline T min_component(const Vector2<T> &v) {
  return std::min(v.x, v.y);
}
template <typename T>
inline T max_component(const Vector2<T> &v) {
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
}  // namespace TRay