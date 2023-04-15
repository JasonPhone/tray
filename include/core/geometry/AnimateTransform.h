/**
 * @file animatetransform.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Animated transformation.
 * @version 0.1
 * @date 2023-03-17
 */
#pragma once
#include "core/math/Matrix.h"
#include "core/geometry/Vector.h"
#include "core/geometry/Quaternion.h"
#include "core/geometry/Transform.h"

namespace TRay {
/// @brief M = TRS when decomposing.
class AnimateTransform {
 public:
  AnimateTransform(const Transform &_trans_st, Float _time_st,
                   const Transform &_trans_ed, Float _time_ed);
  void decompose(const Mat4x4 &mat, Vector3f *T, Quaternion *R, Mat4x4 *S);
  void interpolate(Float time, Transform *t) const;
  Ray operator()(const Ray &r) const;
  Vector3f operator()(const Vector3f &v, Float time) const;
  Point3f operator()(const Point3f &p, Float time) const;
  Bound3f motion_bound(const Bound3f &b) const;
  Bound3f point_bound(const Point3f &p) const;
  std::string to_string() const {
    std::string str = "AnimateTransform from\n" + trans_st.to_string() +
                      " to\n" + trans_ed.to_string() +
                      string_format("in time [%f, %f] ", time_st, time_ed);
    return str;
  }

 private:
  const Transform &trans_st, &trans_ed;
  const Float time_st, time_ed;
  // A transform is decmposed into translation T, rotation R and scale S,
  // interpolation is done separetely and the results are composed back.
  Vector3f T[2];
  Quaternion R[2];
  Mat4x4 S[2];
  // To save time if st and ed are the same.
  bool actually_moving;
  // Special case for animating the bounding box.
  bool has_rotation;
  struct DerivativeTerm {
    DerivativeTerm() {}
    DerivativeTerm(Float c, Float x, Float y, Float z)
        : kc(c), kx(x), ky(y), kz(z) {}
    Float eval(const Point3f &p) const;
    Float kc, kx, ky, kz;
  };
  DerivativeTerm c1[3], c2[3], c3[3], c4[3], c5[3];
};
}  // namespace TRay