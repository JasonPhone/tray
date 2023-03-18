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

// Transform decl.
// ---------------
class Transform {
 public:
  Transform() {}
  Transform(const Mat4x4 &_m, const Mat4x4 &_m_inv) : m(_m), m_inv(_m_inv) {}
  Transform(Float _m[4][4]) : m(_m) { m_inv = mat4x4_inverse(m); }
  Transform(const Mat4x4 &_m) : m(_m) { m_inv = mat4x4_inverse(m); }
  bool operator==(const Transform &other) const;
  bool operator!=(const Transform &other) const { return !(*this == other); }
  bool is_identity() const;
  // bool has_scale() const;
  bool will_swap_hand() const;
  // Transform apply.
  template <typename T>
  Point3<T> operator()(const Point3<T> &p) const;
  template <typename T>
  Vector3<T> operator()(const Vector3<T> &v) const;
  template <typename T>
  Normal3<T> operator()(const Normal3<T> &n) const;
  Ray operator()(const Ray &r) const;
  Bound3f operator()(const Bound3f &b) const;
  Transform operator*(const Transform &t) const;

 private:
  Mat4x4 m, m_inv;
  friend class AnimateTransform;
  friend class Quaternion;
};
// Inlines decl.
// -------------
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
// Transform perspective(Float fov, Float znear, Float zfar);

}  // namespace TRay