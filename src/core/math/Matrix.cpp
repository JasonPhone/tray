#include "core/math/Matrix.h"

namespace TRay {
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
  val[1][0] = v10, val[1][1] = v11, val[1][2] = v12, val[1][3] = v13;
  val[2][0] = v20, val[2][1] = v21, val[2][2] = v22, val[2][3] = v23;
  val[3][0] = v30, val[3][1] = v31, val[3][2] = v32, val[3][3] = v33;
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

Mat4x4 mat4x4_multiply(const Mat4x4 &l, const Mat4x4 &r) {
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
Mat4x4 mat4x4_transpose(const Mat4x4 &mat) {
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
} // namespace TRay