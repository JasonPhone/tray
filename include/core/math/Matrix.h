#pragma once
#include "core/TRay.h"
#include "core/stringformat.h"
namespace TRay {
/// @brief The homogeneous matrix in row first.
/// @note We may need a math library for clean work.
struct Mat4x4 {
  Float val[4][4];
  Mat4x4();
  Mat4x4(Float v[4][4]);
  Mat4x4(Float v00, Float v01, Float v02, Float v03, Float v10, Float v11,
         Float v12, Float v13, Float v20, Float v21, Float v22, Float v23,
         Float v30, Float v31, Float v32, Float v33);
  bool operator==(const Mat4x4 &other) const;
  bool operator!=(const Mat4x4 &other) const { return !(*this == other); }
  bool has_NaN() const {
    for (int i = 0; i < 4; i++) 
      for (int j = 0; j < 4; j++)
        if (std::isnan(val[i][j])) return true;
   return false;
  }
};
Mat4x4 mat4x4_multiply(const Mat4x4 &l, const Mat4x4 &r);
Mat4x4 mat4x4_transpose(const Mat4x4 &m);
Mat4x4 mat4x4_inverse(const Mat4x4 &m);

inline std::ostream &operator<<(std::ostream &os, const Mat4x4 &mat) {
  os << "{\n";
  for (int i = 0; i < 4; i++) {
    os << "\t";
    for (int j = 0; j < 4; j++) {
      os << format_one("%f ", mat.val[i][j]);
    }
    os << "\n";
  }
  os << "}\n";
  return os;
}
} // namespace TRay