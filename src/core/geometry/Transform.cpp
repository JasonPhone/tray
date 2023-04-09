#include "core/geometry/Transform.h"
#include "core/geometry/Vector.h"
#include "core/geometry/Point.h"
#include "core/geometry/Ray.h"
#include "core/geometry/Bound.h"
#include "core/geometry/Interaction.h"

namespace TRay {
bool Transform::operator==(const Transform &other) const {
  return m == other.m;
}
bool Transform::is_identity() const {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (m.val[i][j] != Float(i == j)) {
        return false;
      }
    }
  }
  return true;
}
bool Transform::has_scale() const {
  Float la2 = (*this)(Vector3f(1, 0, 0)).length2();
  Float lb2 = (*this)(Vector3f(0, 1, 0)).length2();
  Float lc2 = (*this)(Vector3f(0, 0, 1)).length2();
#define NOT_ONE(x) ((x) < .999f || (x) > 1.001f)
  return (NOT_ONE(la2) || NOT_ONE(lb2) || NOT_ONE(lc2));
#undef NOT_ONE
}
bool Transform::will_swap_hand() const {
  Float det =
      m.val[0][0] * (m.val[1][1] * m.val[2][2] - m.val[1][2] * m.val[2][1]) +
      m.val[0][1] * (m.val[1][2] * m.val[2][0] - m.val[1][0] * m.val[2][2]) +
      m.val[0][2] * (m.val[1][0] * m.val[2][1] - m.val[1][1] * m.val[2][0]);
  return det < 0;
}
Ray Transform::operator()(const Ray &r) const {
  Vector3f o_err;
  // Point3f o = (*this)(r.ori, &o_err);
  // TODO Revisit this when dealing with rounding error.
  Point3f o = (*this)(r.ori);
  Vector3f d = (*this)(r.dir);
  Float t_m = r.t_max;
  // Offset ray origin to edge of error bounds and compute _tMax_
  // Float lengthSquared = d.length2();
  // if (lengthSquared > 0) {
  //   Float dt = dot(abs(d), o_err) / lengthSquared;
  //   o += d * dt;
  //   t_m -= dt;
  // }
  return Ray(o, d, t_m, r.time);
}
Transform Transform::inverse() const { return Transform(m_inv, m); }
Bound3f Transform::operator()(const Bound3f &b) const {
  // Take every extreme element.
  // https://github.com/erich666/GraphicsGems/blob/master/gems/TransBox.c
  Float pmin[3], pmax[3];
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      Float val1 = b.p_min[j] * m.val[i][j];
      Float val2 = b.p_max[j] * m.val[i][j];
      pmin[i] += std::min(val1, val2);
      pmax[i] += std::max(val1, val2);
    }
  }
  for (int i = 0; i < 3; i++) {
    pmin[i] += m.val[i][3];
    pmax[i] += m.val[i][3];
  }
  Point3f p1(pmin[0], pmin[1], pmin[2]);
  Point3f p2(pmax[0], pmax[1], pmax[2]);
  return Bound3f(p1, p2);
}
SurfaceInteraction Transform::operator()(const SurfaceInteraction &si) const {
  SurfaceInteraction ret;
  const Transform &t = *this;
  ret.p = t(si.p);

  ret.n = normalize(t(si.n));
  ret.wo = normalize(t(si.wo));
  ret.time = si.time;
  ret.uv = si.uv;
  ret.shape = si.shape;
  ret.dpdu = t(si.dpdu);
  ret.dpdv = t(si.dpdv);
  ret.shading.n = normalize(t(si.shading.n));
  ret.shading.dpdu = t(si.shading.dpdu);
  ret.shading.dpdv = t(si.shading.dpdv);
  ret.shading.n = align_with(ret.shading.n, ret.n);
  return ret;
}

Transform Transform::operator*(const Transform &t) const {
  return Transform(mat4x4_multiply(m, t.m), mat4x4_multiply(t.m_inv, m_inv));
}

Transform translate(const Vector3f &delta) {
  // clang-format off
  Mat4x4 m(
    1, 0, 0, delta.x,
    0, 1, 0, delta.y,
    0, 0, 1, delta.z,
    0, 0, 0,       1
  );
  Mat4x4 m_inv(
    1, 0, 0, -delta.x,
    0, 1, 0, -delta.y,
    0, 0, 1, -delta.z,
    0, 0, 0,        1
  );
  return Transform(m, m_inv);
  // clang-format on
}
Transform scale(Float x, Float y, Float z) {
  // clang-format off
  Mat4x4 m(
    x, 0, 0, 0,
    0, y, 0, 0,
    0, 0, z, 0,
    0, 0, 0, 1
  );
  Mat4x4 m_inv(
    1.0/x, 0,     0,     0,
    0,     1.0/y, 0,     0,
    0,     0,     1.0/z, 0,
    0,     0,     0,     1
  );
  return Transform(m, m_inv);
  // clang-format on
}
Transform rotate_x(Float theta) {
  Float sin_theta = std::sin(deg_to_rad(theta));
  Float cos_theta = std::cos(deg_to_rad(theta));
  // clang-format off
  Mat4x4 m(
    1,         0,          0, 0,
    0, cos_theta, -sin_theta, 0,
    0, sin_theta,  cos_theta, 0,
    0,         0,           0, 1
  );
  // clang-format on
  return Transform(m, mat4x4_transpose(m));
}
Transform rotate_y(Float theta) {
  Float sin_theta = std::sin(deg_to_rad(theta));
  Float cos_theta = std::cos(deg_to_rad(theta));
  // clang-format off
  Mat4x4 m(
     cos_theta,         0, sin_theta, 0,
             0,         1,         0, 0,
    -sin_theta,         0, cos_theta, 0,
             0,         0,         0, 1
  );
  // clang-format on
  return Transform(m, mat4x4_transpose(m));
}
Transform rotate_z(Float theta) {
  Float sin_theta = std::sin(deg_to_rad(theta));
  Float cos_theta = std::cos(deg_to_rad(theta));
  // clang-format off
  Mat4x4 m(
    cos_theta, -sin_theta, 0, 0,
    sin_theta,  cos_theta, 0, 0,
            0,          0, 0, 0,
            0,          0, 0, 1
  );
  // clang-format on
  return Transform(m, mat4x4_transpose(m));
}
Transform rotate(Float theta, const Vector3f &axis) {
  Vector3f a = normalize(axis);
  Float sin_theta = std::sin(deg_to_rad(theta));
  Float cos_theta = std::cos(deg_to_rad(theta));
  Mat4x4 m;
  // Rotate the basis vector (1, 0, 0)
  m.val[0][0] = a.x * a.x + (1 - a.x * a.x) * cos_theta;
  m.val[0][1] = a.x * a.y * (1 - cos_theta) - a.z * sin_theta;
  m.val[0][2] = a.x * a.z * (1 - cos_theta) + a.y * sin_theta;
  m.val[0][3] = 0;
  // Rotate the basis vector (0, 1, 0)
  m.val[1][0] = a.x * a.y * (1 - cos_theta) + a.z * sin_theta;
  m.val[1][1] = a.y * a.y + (1 - a.y * a.y) * cos_theta;
  m.val[1][2] = a.y * a.z * (1 - cos_theta) - a.x * sin_theta;
  m.val[1][3] = 0;
  // Rotate the basis vector (0, 0, 1)
  m.val[2][0] = a.x * a.z * (1 - cos_theta) - a.y * sin_theta;
  m.val[2][1] = a.y * a.z * (1 - cos_theta) + a.x * sin_theta;
  m.val[2][2] = a.z * a.z + (1 - a.z * a.z) * cos_theta;
  m.val[2][3] = 0;
  return Transform(m, mat4x4_transpose(m));
}
Transform look_at(const Point3f &eye_pos, const Point3f &look,
                  const Vector3f &up_dir) {
  Mat4x4 camera_to_world;  // Camera space to world space.
  // Fourth column of viewing matrix.
  camera_to_world.val[0][3] = eye_pos.x;
  camera_to_world.val[1][3] = eye_pos.y;
  camera_to_world.val[2][3] = eye_pos.z;
  camera_to_world.val[3][3] = 1;

  // First three columns of viewing matrix.
  Vector3f dir = normalize(look - eye_pos);
  if (cross(normalize(up_dir), dir).length() == 0) {
    SError(
        string_format("TRay::look_at: Same direction of up vector (%f, %f, %f) "
                      "and viewing direction (%f, %f, %f). "
                      "Using identity transformation.",
                      up_dir.x, up_dir.y, up_dir.z, dir.x, dir.y, dir.z));
    return Transform();
  }
  Vector3f right = normalize(cross(normalize(up_dir), dir));
  Vector3f up_new = cross(dir, right);
  camera_to_world.val[0][0] = right.x;
  camera_to_world.val[1][0] = right.y;
  camera_to_world.val[2][0] = right.z;
  camera_to_world.val[3][0] = 0.;

  camera_to_world.val[0][1] = up_new.x;
  camera_to_world.val[1][1] = up_new.y;
  camera_to_world.val[2][1] = up_new.z;
  camera_to_world.val[3][1] = 0.;

  camera_to_world.val[0][2] = dir.x;
  camera_to_world.val[1][2] = dir.y;
  camera_to_world.val[2][2] = dir.z;
  camera_to_world.val[3][2] = 0.;
  return Transform(mat4x4_inverse(camera_to_world), camera_to_world);
}
// Transform orthographic(Float z_near, Float z_far) {
// return scale(1, 1, 1 / (z_far - z_near)) * translate(Vector3f(0, 0,
// -z_near));
// }
Transform perspective(Float fov, Float znear, Float zfar) {
  // Projective divide for perspective projection.
  // clang-format off
  Mat4x4 persp(1, 0,                     0,                              0,
               0, 1,                     0,                              0,
               0, 0, zfar / (zfar - znear), -zfar * znear / (zfar - znear),
               0, 0,                     1,                              0);
  // clang-format on
  // Scale canonical perspective view to specified field of view
  Float tan_ang_inv = 1 / std::tan(deg_to_rad(fov) / 2);
  return scale(tan_ang_inv, tan_ang_inv, 1) * Transform(persp);
}

}  // namespace TRay