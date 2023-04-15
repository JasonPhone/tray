/**
 * @file transform.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Static transformation.
 * @version 0.1
 * @date 2023-03-16
 */
#pragma once
#include "core/TRay.h"
#include "core/math/Matrix.h"
#include "core/stringformat.h"

/**
 * Transform is a mapping. Here we consider transformations which are:
 *  1. Linear.
 *  2. Continuous.
 *  3. Invertible.
 * Further info may be included in geometry field of graphics.
 * The math of transformation is about homogeneous coordinates.
 *
 * There are 2 INCOMPATIBLE ways to interpret a transform matrix:
 *  1. Under the same frame, transform one point/vector to a new one.
 *  2. With the same point/vector, calculate the new coordinates under a new
 * frame. We choose the latter one as most of our work can be done nicely if we
 * pick an appropriate frame. For example the camera space in OpenGL.
 *
 * We assign each shape an individual transformation in order to simplify the
 * scene description file.
 */

namespace TRay {
class Transform {
 public:
  Transform() {}
  Transform(const Mat4x4 &_m, const Mat4x4 &_m_inv) : m(_m), m_inv(_m_inv) {
    ASSERT(!m.has_NaN() && !m_inv.has_NaN());
  }
  Transform(Float _m[4][4]) : m(_m) {
    m_inv = mat4x4_inverse(m);
    ASSERT(!m.has_NaN() && !m_inv.has_NaN());
  }
  Transform(const Mat4x4 &_m) : m(_m) {
    m_inv = mat4x4_inverse(m);
    ASSERT(!m.has_NaN() && !m_inv.has_NaN());
  }
  bool operator==(const Transform &other) const;
  bool operator!=(const Transform &other) const { return !(*this == other); }
  bool is_identity() const;
  bool has_scale() const;
  bool will_swap_hand() const;
  Transform inverse() const;
  // Transform apply.
  template <typename T>
  Point3<T> operator()(const Point3<T> &p) const {
    T x = p.x, y = p.y, z = p.z;
    // clang-format off
    T xp = m.val[0][0] * x + m.val[0][1] * y + m.val[0][2] * z + m.val[0][3];
    T yp = m.val[1][0] * x + m.val[1][1] * y + m.val[1][2] * z + m.val[1][3];
    T zp = m.val[2][0] * x + m.val[2][1] * y + m.val[2][2] * z + m.val[2][3];
    T wp = m.val[3][0] * x + m.val[3][1] * y + m.val[3][2] * z + m.val[3][3];
    // clang-format on
    ASSERT(wp != 0);
    if (wp == 1)
      return Point3<T>(xp, yp, zp);
    else
      return Point3<T>(xp, yp, zp) * (1.0 / wp);
  }
  template <typename T>
  Vector3<T> operator()(const Vector3<T> &v) const {
    T pv[3] = {v.x, v.y, v.z};
    T p_new[3] = {0, 0, 0};
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        p_new[i] += m.val[i][j] * pv[j];
      }
    }
    return Vector3<T>(p_new[0], p_new[1], p_new[2]);
  }
  /// @brief Some linear transform can show this.
  template <typename T>
  Normal3<T> operator()(const Normal3<T> &n) const {
    T x = n.x, y = n.y, z = n.z;
    return Normal3<T>(
        m_inv.val[0][0] * x + m_inv.val[1][0] * y + m_inv.val[2][0] * z,
        m_inv.val[0][1] * x + m_inv.val[1][1] * y + m_inv.val[2][1] * z,
        m_inv.val[0][2] * x + m_inv.val[1][2] * y + m_inv.val[2][2] * z);
  }
  Ray operator()(const Ray &r) const;
  Bound3f operator()(const Bound3f &b) const;
  SurfaceInteraction operator()(const SurfaceInteraction &si) const;
  Transform operator*(const Transform &t) const;
  std::string to_string() const {
    std::string str = "t = " + m.to_string() + ", inv = " + m_inv.to_string();
    return to_string();
  }
  friend std::ostream &operator<<(std::ostream &os, const Transform &t) {
    os << t.to_string();
    return os;
  }

 private:
  Mat4x4 m, m_inv;
  friend class AnimateTransform;
  friend class Quaternion;
};

Transform translate(const Vector3f &delta);
Transform scale(Float x, Float y, Float z);
Transform rotate_x(Float theta);
Transform rotate_y(Float theta);
Transform rotate_z(Float theta);
Transform rotate(Float theta, const Vector3f &axis);
/// @brief OpenGL-style viewing matrix.
Transform look_at(const Point3f &eye_pos, const Point3f &look,
                  const Vector3f &up_dir);
// Transform orthographic(Float z_near, Float z_far);
Transform perspective(Float fov, Float znear, Float zfar);

}  // namespace TRay