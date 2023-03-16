#include "core/geometry/transform.h"
#include "core/geometry/vector.h"
#include "core/geometry/point.h"

namespace TRay {
// Mat4x4 impl.
// ------------
Mat4x4::Mat4x4() {
  val[0][0] = val[1][1] = val[2][2] = val[3][3] = 1.0;
  val[0][1] = val[0][2] = val[0][3] = 0.0;
  val[1][0] = val[1][2] = val[1][3] = 0.0;
  val[2][0] = val[2][1] = val[2][3] = 0.0;
  val[3][0] = val[3][1] = val[3][2] = 0.0;
}
Mat4x4::Mat4x4(Float v[4][4]) { memcpy(this->val, v, 16 * sizeof(Float)); }
// clang-format off
Mat4x4::Mat4x4(Float v00, Float v01, Float v02, Float v03,
               Float v10, Float v11, Float v12, Float v13,
               Float v20, Float v21, Float v22, Float v23,
               Float v30, Float v31, Float v32, Float v33) {
  val[0][0] = v00, val[0][1] = v01, val[0][2] = v02, val[0][3] = v03;
  val[1][0] = v10, val[1][1] = v11, val[1][2] = v12, val[1][3] = v03;
  val[2][0] = v20, val[2][1] = v21, val[2][2] = v22, val[2][3] = v03;
  val[3][0] = v30, val[3][1] = v31, val[3][2] = v32, val[3][3] = v03;
}
// clang-format on
bool Mat4x4::operator==(const Mat4x4 &other) const {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (val[i][j] != other.val[i][j]) return false;
    }
  }
  return true;
}

// Mat4x4 inlines.
// ---------------
inline Mat4x4 mat4x4_multiply(const Mat4x4 &l, const Mat4x4 &r) {
  Mat4x4 ans;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ans.val[i][j] = 0;
      for (int k = 0; k < 4; k++) {
        ans.val[i][j] += l.val[i][k] * r.val[j][k];
      }
    }
  }
  return ans;
}
inline Mat4x4 mat4x4_transpose(const Mat4x4 &mat) {
  Mat4x4 ret;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ret.val[j][i] = mat.val[i][j];
    }
  }
  return ret;
}
Mat4x4 mat4x4_inverse(const Mat4x4 &mat) {
  Float m[4][4];                  // Operate on this.
  int col_idx[4], row_idx[4];     // Store the swap history.
  int piv_idx[4] = {0, 0, 0, 0};  // Pivot position counter.
                                  // A pivot is always on diagonal.
  memcpy(m, mat.val, 16 * sizeof(Float));
  for (int i = 0; i < 4; i++) {
    int irow = 0, icol = 0;
    Float maxv = 0.0;
    // Choose pivot.
    for (int j = 0; j < 4; j++) {
      // if (piv_idx[j] != 1) {
      if (!piv_idx[j]) {
        // This row has no pivot or over picked.
        for (int k = 0; k < 4; k++) {
          // if (piv_idx[k] > 1) {
          //   // Over picked.
          //   SError("Singular matrix has no inversion!");
          // } else
          if (piv_idx[k] == 0) {
            // This column has no pivot.
            // Seek maximum in this row.
            if (std::abs(m[j][k]) >= maxv) {
              maxv = Float(std::abs(m[j][k]));
              irow = j;
              icol = k;
            }
          }
        }
      }
    }
    // Over picked.
    if (piv_idx[icol])
      SError("TRay::mat4x4_inverse: Trying to inverse singular matrix.");
    ++piv_idx[icol];
    // Swap to the diagonal for pivot.
    if (irow != icol) {
      for (int k = 0; k < 4; ++k) std::swap(m[irow][k], m[icol][k]);
    }
    // Record the swapping.
    row_idx[i] = irow;
    col_idx[i] = icol;
    if (m[icol][icol] == 0.f)
      SError("TRay::mat4x4_inverse: Trying to inverse singular matrix.");

    // Set the pivot to 1
    Float pivinv = 1. / m[icol][icol];
    m[icol][icol] = 1.;  // Store back to the original matrix.
    for (int j = 0; j < 4; j++) m[icol][j] *= pivinv;

    // Zero out the other columns of the other rows.
    for (int j = 0; j < 4; j++) {
      if (j != icol) {
        Float save = m[j][icol];
        m[j][icol] = 0;  // Store back to the original matrix.
        for (int k = 0; k < 4; k++) m[j][k] -= m[icol][k] * save;
      }
    }
  }
  // Swap columns back to cancel the swapping.
  for (int j = 3; j >= 0; j--) {
    if (row_idx[j] != col_idx[j]) {
      for (int k = 0; k < 4; k++) std::swap(m[k][row_idx[j]], m[k][col_idx[j]]);
    }
  }
  return Mat4x4(m);
}

// Transform impl.
// ---------------
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
// Inlines impl.
// -------------
Transform translate(const Vector3f &delta) {
  // clang-format off
  Mat4x4 m(
    0, 0, 0, delta.x,
    0, 0, 0, delta.y,
    0, 0, 0, delta.z,
    0, 0, 0,       1
  );
  Mat4x4 m_inv(
    0, 0, 0, -delta.x,
    0, 0, 0, -delta.y,
    0, 0, 0, -delta.z,
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
    FError(
        "TRay::look_at: Same direction of up vector (%f, %f, %f) "
        "and viewing direction (%f, %f, %f). "
        "Using identity transformation.",
        up_dir.x, up_dir.y, up_dir.z, dir.x, dir.y, dir.z);
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
// return scale(1, 1, 1 / (z_far - z_near)) * translate(Vector3f(0, 0, -z_near));
// }
// Transform perspective(Float fov, Float znear, Float zfar);
}  // namespace TRay