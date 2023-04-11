#include "core/geometry/AnimateTransform.h"
#include "core/geometry/Transform.h"
#include "core/math/Matrix.h"
#include "core/geometry/Vector.h"
#include "core/geometry/Point.h"
#include "core/geometry/Ray.h"
#include "core/geometry/Bound.h"

namespace TRay {
class Interval {
 public:
  // Interval Public Methods
  Interval(Float v) : low(v), high(v) {}
  Interval(Float v0, Float v1)
      : low(std::min(v0, v1)), high(std::max(v0, v1)) {}
  Interval operator+(const Interval &i) const {
    return Interval(low + i.low, high + i.high);
  }
  Interval operator-(const Interval &i) const {
    return Interval(low - i.high, high - i.low);
  }
  Interval operator*(const Interval &i) const {
    return Interval(std::min(std::min(low * i.low, high * i.low),
                             std::min(low * i.high, high * i.high)),
                    std::max(std::max(low * i.low, high * i.low),
                             std::max(low * i.high, high * i.high)));
  }
  Float low, high;
};

Interval sin(const Interval &i) {
  ASSERT(i.low >= 0);
  ASSERT(i.high <= 2.0001 * PI);
  Float sin_low = std::sin(i.low), sin_high = std::sin(i.high);
  if (sin_low > sin_high) std::swap(sin_low, sin_high);
  if (i.low < PI / 2 && i.high > PI / 2) sin_high = 1.;
  if (i.low < (3.f / 2.f) * PI && i.high > (3.f / 2.f) * PI) sin_low = -1.;
  return Interval(sin_low, sin_high);
}
Interval cos(const Interval &i) {
  ASSERT(i.low >= 0);
  ASSERT(i.high <= 2.0001 * PI);
  Float cos_low = std::cos(i.low), cos_high = std::cos(i.high);
  if (cos_low > cos_high) std::swap(cos_low, cos_high);
  if (i.low < PI && i.high > PI) cos_low = -1.;
  return Interval(cos_low, cos_high);
}
void interval_find_zeros(Float c1, Float c2, Float c3, Float c4, Float c5,
                         Float theta, Interval t_intvl, Float *zeros,
                         int *n_zero, int depth = 8) {
  // Evaluate motion derivative in interval form, return if no zeros
  Interval range = Interval(c1) +
                   (Interval(c2) + Interval(c3) * t_intvl) *
                       cos(Interval(2 * theta) * t_intvl) +
                   (Interval(c4) + Interval(c5) * t_intvl) *
                       sin(Interval(2 * theta) * t_intvl);
  if (range.low > 0. || range.high < 0. || range.low == range.high) return;
  if (depth > 0) {
    // Split _tInterval_ and check both resulting intervals
    Float mid = (t_intvl.low + t_intvl.high) * 0.5f;
    interval_find_zeros(c1, c2, c3, c4, c5, theta, Interval(t_intvl.low, mid),
                        zeros, n_zero, depth - 1);
    interval_find_zeros(c1, c2, c3, c4, c5, theta, Interval(mid, t_intvl.high),
                        zeros, n_zero, depth - 1);
  } else {
    // Use Newton's method to refine zero
    Float t_newton = (t_intvl.low + t_intvl.high) * 0.5f;
    for (int i = 0; i < 4; ++i) {
      Float f_newton = c1 +
                       (c2 + c3 * t_newton) * std::cos(2.f * theta * t_newton) +
                       (c4 + c5 * t_newton) * std::sin(2.f * theta * t_newton);
      Float f_prime_newton = (c3 + 2 * (c4 + c5 * t_newton) * theta) *
                                 std::cos(2.f * t_newton * theta) +
                             (c5 - 2 * (c2 + c3 * t_newton) * theta) *
                                 std::sin(2.f * t_newton * theta);
      if (f_newton == 0 || f_prime_newton == 0) break;
      t_newton = t_newton - f_newton / f_prime_newton;
    }
    if (t_newton >= t_intvl.low - 1e-3f && t_newton < t_intvl.high + 1e-3f) {
      zeros[*n_zero] = t_newton;
      (*n_zero)++;
    }
  }
}

AnimateTransform::AnimateTransform(const Transform &_trans_st, Float _time_st,
                                   const Transform &_trans_ed, Float _time_ed)
    : trans_st(_trans_st),
      trans_ed(_trans_ed),
      time_st(_time_st),
      time_ed(_time_ed),
      actually_moving(_trans_st != _trans_ed) {
  decompose(trans_st.m, &(T[0]), &(R[0]), &(S[0]));
  decompose(trans_ed.m, &(T[1]), &(R[1]), &(S[1]));
  // Pick the shortest rotation.
  Float rot = dot(R[0], R[1]);
  if (rot < 0) R[1] = -R[1];
  has_rotation = rot < 0.9995;
  // Compute terms of motion derivative function
  if (has_rotation) {
    Float cos_theta = dot(R[0], R[1]);
    Float theta = std::acos(clamp(cos_theta, -1, 1));
    Quaternion qperp = normalize(R[1] - R[0] * cos_theta);

    Float t0x = T[0].x;
    Float t0y = T[0].y;
    Float t0z = T[0].z;
    Float t1x = T[1].x;
    Float t1y = T[1].y;
    Float t1z = T[1].z;
    Float q0x = R[0].imag.x;
    Float q0y = R[0].imag.y;
    Float q0z = R[0].imag.z;
    Float q0w = R[0].real;
    Float qperpx = qperp.imag.x;
    Float qperpy = qperp.imag.y;
    Float qperpz = qperp.imag.z;
    Float qperpw = qperp.real;
    Float s000 = S[0].val[0][0];
    Float s001 = S[0].val[0][1];
    Float s002 = S[0].val[0][2];
    Float s010 = S[0].val[1][0];
    Float s011 = S[0].val[1][1];
    Float s012 = S[0].val[1][2];
    Float s020 = S[0].val[2][0];
    Float s021 = S[0].val[2][1];
    Float s022 = S[0].val[2][2];
    Float s100 = S[1].val[0][0];
    Float s101 = S[1].val[0][1];
    Float s102 = S[1].val[0][2];
    Float s110 = S[1].val[1][0];
    Float s111 = S[1].val[1][1];
    Float s112 = S[1].val[1][2];
    Float s120 = S[1].val[2][0];
    Float s121 = S[1].val[2][1];
    Float s122 = S[1].val[2][2];

    c1[0] = DerivativeTerm(
        -t0x + t1x,
        (-1 + q0y * q0y + q0z * q0z + qperpy * qperpy + qperpz * qperpz) *
                s000 +
            q0w * q0z * s010 - qperpx * qperpy * s010 + qperpw * qperpz * s010 -
            q0w * q0y * s020 - qperpw * qperpy * s020 - qperpx * qperpz * s020 +
            s100 - q0y * q0y * s100 - q0z * q0z * s100 -
            qperpy * qperpy * s100 - qperpz * qperpz * s100 - q0w * q0z * s110 +
            qperpx * qperpy * s110 - qperpw * qperpz * s110 + q0w * q0y * s120 +
            qperpw * qperpy * s120 + qperpx * qperpz * s120 +
            q0x * (-(q0y * s010) - q0z * s020 + q0y * s110 + q0z * s120),
        (-1 + q0y * q0y + q0z * q0z + qperpy * qperpy + qperpz * qperpz) *
                s001 +
            q0w * q0z * s011 - qperpx * qperpy * s011 + qperpw * qperpz * s011 -
            q0w * q0y * s021 - qperpw * qperpy * s021 - qperpx * qperpz * s021 +
            s101 - q0y * q0y * s101 - q0z * q0z * s101 -
            qperpy * qperpy * s101 - qperpz * qperpz * s101 - q0w * q0z * s111 +
            qperpx * qperpy * s111 - qperpw * qperpz * s111 + q0w * q0y * s121 +
            qperpw * qperpy * s121 + qperpx * qperpz * s121 +
            q0x * (-(q0y * s011) - q0z * s021 + q0y * s111 + q0z * s121),
        (-1 + q0y * q0y + q0z * q0z + qperpy * qperpy + qperpz * qperpz) *
                s002 +
            q0w * q0z * s012 - qperpx * qperpy * s012 + qperpw * qperpz * s012 -
            q0w * q0y * s022 - qperpw * qperpy * s022 - qperpx * qperpz * s022 +
            s102 - q0y * q0y * s102 - q0z * q0z * s102 -
            qperpy * qperpy * s102 - qperpz * qperpz * s102 - q0w * q0z * s112 +
            qperpx * qperpy * s112 - qperpw * qperpz * s112 + q0w * q0y * s122 +
            qperpw * qperpy * s122 + qperpx * qperpz * s122 +
            q0x * (-(q0y * s012) - q0z * s022 + q0y * s112 + q0z * s122));

    c2[0] = DerivativeTerm(
        0.,
        -(qperpy * qperpy * s000) - qperpz * qperpz * s000 +
            qperpx * qperpy * s010 - qperpw * qperpz * s010 +
            qperpw * qperpy * s020 + qperpx * qperpz * s020 +
            q0y * q0y * (s000 - s100) + q0z * q0z * (s000 - s100) +
            qperpy * qperpy * s100 + qperpz * qperpz * s100 -
            qperpx * qperpy * s110 + qperpw * qperpz * s110 -
            qperpw * qperpy * s120 - qperpx * qperpz * s120 +
            2 * q0x * qperpy * s010 * theta - 2 * q0w * qperpz * s010 * theta +
            2 * q0w * qperpy * s020 * theta + 2 * q0x * qperpz * s020 * theta +
            q0y * (q0x * (-s010 + s110) + q0w * (-s020 + s120) +
                   2 * (-2 * qperpy * s000 + qperpx * s010 + qperpw * s020) *
                       theta) +
            q0z * (q0w * (s010 - s110) + q0x * (-s020 + s120) -
                   2 * (2 * qperpz * s000 + qperpw * s010 - qperpx * s020) *
                       theta),
        -(qperpy * qperpy * s001) - qperpz * qperpz * s001 +
            qperpx * qperpy * s011 - qperpw * qperpz * s011 +
            qperpw * qperpy * s021 + qperpx * qperpz * s021 +
            q0y * q0y * (s001 - s101) + q0z * q0z * (s001 - s101) +
            qperpy * qperpy * s101 + qperpz * qperpz * s101 -
            qperpx * qperpy * s111 + qperpw * qperpz * s111 -
            qperpw * qperpy * s121 - qperpx * qperpz * s121 +
            2 * q0x * qperpy * s011 * theta - 2 * q0w * qperpz * s011 * theta +
            2 * q0w * qperpy * s021 * theta + 2 * q0x * qperpz * s021 * theta +
            q0y * (q0x * (-s011 + s111) + q0w * (-s021 + s121) +
                   2 * (-2 * qperpy * s001 + qperpx * s011 + qperpw * s021) *
                       theta) +
            q0z * (q0w * (s011 - s111) + q0x * (-s021 + s121) -
                   2 * (2 * qperpz * s001 + qperpw * s011 - qperpx * s021) *
                       theta),
        -(qperpy * qperpy * s002) - qperpz * qperpz * s002 +
            qperpx * qperpy * s012 - qperpw * qperpz * s012 +
            qperpw * qperpy * s022 + qperpx * qperpz * s022 +
            q0y * q0y * (s002 - s102) + q0z * q0z * (s002 - s102) +
            qperpy * qperpy * s102 + qperpz * qperpz * s102 -
            qperpx * qperpy * s112 + qperpw * qperpz * s112 -
            qperpw * qperpy * s122 - qperpx * qperpz * s122 +
            2 * q0x * qperpy * s012 * theta - 2 * q0w * qperpz * s012 * theta +
            2 * q0w * qperpy * s022 * theta + 2 * q0x * qperpz * s022 * theta +
            q0y * (q0x * (-s012 + s112) + q0w * (-s022 + s122) +
                   2 * (-2 * qperpy * s002 + qperpx * s012 + qperpw * s022) *
                       theta) +
            q0z * (q0w * (s012 - s112) + q0x * (-s022 + s122) -
                   2 * (2 * qperpz * s002 + qperpw * s012 - qperpx * s022) *
                       theta));

    c3[0] = DerivativeTerm(
        0.,
        -2 *
            (q0x * qperpy * s010 - q0w * qperpz * s010 + q0w * qperpy * s020 +
             q0x * qperpz * s020 - q0x * qperpy * s110 + q0w * qperpz * s110 -
             q0w * qperpy * s120 - q0x * qperpz * s120 +
             q0y * (-2 * qperpy * s000 + qperpx * s010 + qperpw * s020 +
                    2 * qperpy * s100 - qperpx * s110 - qperpw * s120) +
             q0z * (-2 * qperpz * s000 - qperpw * s010 + qperpx * s020 +
                    2 * qperpz * s100 + qperpw * s110 - qperpx * s120)) *
            theta,
        -2 *
            (q0x * qperpy * s011 - q0w * qperpz * s011 + q0w * qperpy * s021 +
             q0x * qperpz * s021 - q0x * qperpy * s111 + q0w * qperpz * s111 -
             q0w * qperpy * s121 - q0x * qperpz * s121 +
             q0y * (-2 * qperpy * s001 + qperpx * s011 + qperpw * s021 +
                    2 * qperpy * s101 - qperpx * s111 - qperpw * s121) +
             q0z * (-2 * qperpz * s001 - qperpw * s011 + qperpx * s021 +
                    2 * qperpz * s101 + qperpw * s111 - qperpx * s121)) *
            theta,
        -2 *
            (q0x * qperpy * s012 - q0w * qperpz * s012 + q0w * qperpy * s022 +
             q0x * qperpz * s022 - q0x * qperpy * s112 + q0w * qperpz * s112 -
             q0w * qperpy * s122 - q0x * qperpz * s122 +
             q0y * (-2 * qperpy * s002 + qperpx * s012 + qperpw * s022 +
                    2 * qperpy * s102 - qperpx * s112 - qperpw * s122) +
             q0z * (-2 * qperpz * s002 - qperpw * s012 + qperpx * s022 +
                    2 * qperpz * s102 + qperpw * s112 - qperpx * s122)) *
            theta);

    c4[0] = DerivativeTerm(
        0.,
        -(q0x * qperpy * s010) + q0w * qperpz * s010 - q0w * qperpy * s020 -
            q0x * qperpz * s020 + q0x * qperpy * s110 - q0w * qperpz * s110 +
            q0w * qperpy * s120 + q0x * qperpz * s120 +
            2 * q0y * q0y * s000 * theta + 2 * q0z * q0z * s000 * theta -
            2 * qperpy * qperpy * s000 * theta -
            2 * qperpz * qperpz * s000 * theta +
            2 * qperpx * qperpy * s010 * theta -
            2 * qperpw * qperpz * s010 * theta +
            2 * qperpw * qperpy * s020 * theta +
            2 * qperpx * qperpz * s020 * theta +
            q0y * (-(qperpx * s010) - qperpw * s020 +
                   2 * qperpy * (s000 - s100) + qperpx * s110 + qperpw * s120 -
                   2 * q0x * s010 * theta - 2 * q0w * s020 * theta) +
            q0z * (2 * qperpz * s000 + qperpw * s010 - qperpx * s020 -
                   2 * qperpz * s100 - qperpw * s110 + qperpx * s120 +
                   2 * q0w * s010 * theta - 2 * q0x * s020 * theta),
        -(q0x * qperpy * s011) + q0w * qperpz * s011 - q0w * qperpy * s021 -
            q0x * qperpz * s021 + q0x * qperpy * s111 - q0w * qperpz * s111 +
            q0w * qperpy * s121 + q0x * qperpz * s121 +
            2 * q0y * q0y * s001 * theta + 2 * q0z * q0z * s001 * theta -
            2 * qperpy * qperpy * s001 * theta -
            2 * qperpz * qperpz * s001 * theta +
            2 * qperpx * qperpy * s011 * theta -
            2 * qperpw * qperpz * s011 * theta +
            2 * qperpw * qperpy * s021 * theta +
            2 * qperpx * qperpz * s021 * theta +
            q0y * (-(qperpx * s011) - qperpw * s021 +
                   2 * qperpy * (s001 - s101) + qperpx * s111 + qperpw * s121 -
                   2 * q0x * s011 * theta - 2 * q0w * s021 * theta) +
            q0z * (2 * qperpz * s001 + qperpw * s011 - qperpx * s021 -
                   2 * qperpz * s101 - qperpw * s111 + qperpx * s121 +
                   2 * q0w * s011 * theta - 2 * q0x * s021 * theta),
        -(q0x * qperpy * s012) + q0w * qperpz * s012 - q0w * qperpy * s022 -
            q0x * qperpz * s022 + q0x * qperpy * s112 - q0w * qperpz * s112 +
            q0w * qperpy * s122 + q0x * qperpz * s122 +
            2 * q0y * q0y * s002 * theta + 2 * q0z * q0z * s002 * theta -
            2 * qperpy * qperpy * s002 * theta -
            2 * qperpz * qperpz * s002 * theta +
            2 * qperpx * qperpy * s012 * theta -
            2 * qperpw * qperpz * s012 * theta +
            2 * qperpw * qperpy * s022 * theta +
            2 * qperpx * qperpz * s022 * theta +
            q0y * (-(qperpx * s012) - qperpw * s022 +
                   2 * qperpy * (s002 - s102) + qperpx * s112 + qperpw * s122 -
                   2 * q0x * s012 * theta - 2 * q0w * s022 * theta) +
            q0z * (2 * qperpz * s002 + qperpw * s012 - qperpx * s022 -
                   2 * qperpz * s102 - qperpw * s112 + qperpx * s122 +
                   2 * q0w * s012 * theta - 2 * q0x * s022 * theta));

    c5[0] = DerivativeTerm(
        0.,
        2 *
            (qperpy * qperpy * s000 + qperpz * qperpz * s000 -
             qperpx * qperpy * s010 + qperpw * qperpz * s010 -
             qperpw * qperpy * s020 - qperpx * qperpz * s020 -
             qperpy * qperpy * s100 - qperpz * qperpz * s100 +
             q0y * q0y * (-s000 + s100) + q0z * q0z * (-s000 + s100) +
             qperpx * qperpy * s110 - qperpw * qperpz * s110 +
             q0y * (q0x * (s010 - s110) + q0w * (s020 - s120)) +
             qperpw * qperpy * s120 + qperpx * qperpz * s120 +
             q0z * (-(q0w * s010) + q0x * s020 + q0w * s110 - q0x * s120)) *
            theta,
        2 *
            (qperpy * qperpy * s001 + qperpz * qperpz * s001 -
             qperpx * qperpy * s011 + qperpw * qperpz * s011 -
             qperpw * qperpy * s021 - qperpx * qperpz * s021 -
             qperpy * qperpy * s101 - qperpz * qperpz * s101 +
             q0y * q0y * (-s001 + s101) + q0z * q0z * (-s001 + s101) +
             qperpx * qperpy * s111 - qperpw * qperpz * s111 +
             q0y * (q0x * (s011 - s111) + q0w * (s021 - s121)) +
             qperpw * qperpy * s121 + qperpx * qperpz * s121 +
             q0z * (-(q0w * s011) + q0x * s021 + q0w * s111 - q0x * s121)) *
            theta,
        2 *
            (qperpy * qperpy * s002 + qperpz * qperpz * s002 -
             qperpx * qperpy * s012 + qperpw * qperpz * s012 -
             qperpw * qperpy * s022 - qperpx * qperpz * s022 -
             qperpy * qperpy * s102 - qperpz * qperpz * s102 +
             q0y * q0y * (-s002 + s102) + q0z * q0z * (-s002 + s102) +
             qperpx * qperpy * s112 - qperpw * qperpz * s112 +
             q0y * (q0x * (s012 - s112) + q0w * (s022 - s122)) +
             qperpw * qperpy * s122 + qperpx * qperpz * s122 +
             q0z * (-(q0w * s012) + q0x * s022 + q0w * s112 - q0x * s122)) *
            theta);

    c1[1] = DerivativeTerm(
        -t0y + t1y,
        -(qperpx * qperpy * s000) - qperpw * qperpz * s000 - s010 +
            q0z * q0z * s010 + qperpx * qperpx * s010 + qperpz * qperpz * s010 -
            q0y * q0z * s020 + qperpw * qperpx * s020 - qperpy * qperpz * s020 +
            qperpx * qperpy * s100 + qperpw * qperpz * s100 +
            q0w * q0z * (-s000 + s100) + q0x * q0x * (s010 - s110) + s110 -
            q0z * q0z * s110 - qperpx * qperpx * s110 - qperpz * qperpz * s110 +
            q0x * (q0y * (-s000 + s100) + q0w * (s020 - s120)) +
            q0y * q0z * s120 - qperpw * qperpx * s120 + qperpy * qperpz * s120,
        -(qperpx * qperpy * s001) - qperpw * qperpz * s001 - s011 +
            q0z * q0z * s011 + qperpx * qperpx * s011 + qperpz * qperpz * s011 -
            q0y * q0z * s021 + qperpw * qperpx * s021 - qperpy * qperpz * s021 +
            qperpx * qperpy * s101 + qperpw * qperpz * s101 +
            q0w * q0z * (-s001 + s101) + q0x * q0x * (s011 - s111) + s111 -
            q0z * q0z * s111 - qperpx * qperpx * s111 - qperpz * qperpz * s111 +
            q0x * (q0y * (-s001 + s101) + q0w * (s021 - s121)) +
            q0y * q0z * s121 - qperpw * qperpx * s121 + qperpy * qperpz * s121,
        -(qperpx * qperpy * s002) - qperpw * qperpz * s002 - s012 +
            q0z * q0z * s012 + qperpx * qperpx * s012 + qperpz * qperpz * s012 -
            q0y * q0z * s022 + qperpw * qperpx * s022 - qperpy * qperpz * s022 +
            qperpx * qperpy * s102 + qperpw * qperpz * s102 +
            q0w * q0z * (-s002 + s102) + q0x * q0x * (s012 - s112) + s112 -
            q0z * q0z * s112 - qperpx * qperpx * s112 - qperpz * qperpz * s112 +
            q0x * (q0y * (-s002 + s102) + q0w * (s022 - s122)) +
            q0y * q0z * s122 - qperpw * qperpx * s122 + qperpy * qperpz * s122);

    c2[1] = DerivativeTerm(
        0.,
        qperpx * qperpy * s000 + qperpw * qperpz * s000 + q0z * q0z * s010 -
            qperpx * qperpx * s010 - qperpz * qperpz * s010 - q0y * q0z * s020 -
            qperpw * qperpx * s020 + qperpy * qperpz * s020 -
            qperpx * qperpy * s100 - qperpw * qperpz * s100 +
            q0x * q0x * (s010 - s110) - q0z * q0z * s110 +
            qperpx * qperpx * s110 + qperpz * qperpz * s110 + q0y * q0z * s120 +
            qperpw * qperpx * s120 - qperpy * qperpz * s120 +
            2 * q0z * qperpw * s000 * theta + 2 * q0y * qperpx * s000 * theta -
            4 * q0z * qperpz * s010 * theta + 2 * q0z * qperpy * s020 * theta +
            2 * q0y * qperpz * s020 * theta +
            q0x * (q0w * s020 + q0y * (-s000 + s100) - q0w * s120 +
                   2 * qperpy * s000 * theta - 4 * qperpx * s010 * theta -
                   2 * qperpw * s020 * theta) +
            q0w * (-(q0z * s000) + q0z * s100 + 2 * qperpz * s000 * theta -
                   2 * qperpx * s020 * theta),
        qperpx * qperpy * s001 + qperpw * qperpz * s001 + q0z * q0z * s011 -
            qperpx * qperpx * s011 - qperpz * qperpz * s011 - q0y * q0z * s021 -
            qperpw * qperpx * s021 + qperpy * qperpz * s021 -
            qperpx * qperpy * s101 - qperpw * qperpz * s101 +
            q0x * q0x * (s011 - s111) - q0z * q0z * s111 +
            qperpx * qperpx * s111 + qperpz * qperpz * s111 + q0y * q0z * s121 +
            qperpw * qperpx * s121 - qperpy * qperpz * s121 +
            2 * q0z * qperpw * s001 * theta + 2 * q0y * qperpx * s001 * theta -
            4 * q0z * qperpz * s011 * theta + 2 * q0z * qperpy * s021 * theta +
            2 * q0y * qperpz * s021 * theta +
            q0x * (q0w * s021 + q0y * (-s001 + s101) - q0w * s121 +
                   2 * qperpy * s001 * theta - 4 * qperpx * s011 * theta -
                   2 * qperpw * s021 * theta) +
            q0w * (-(q0z * s001) + q0z * s101 + 2 * qperpz * s001 * theta -
                   2 * qperpx * s021 * theta),
        qperpx * qperpy * s002 + qperpw * qperpz * s002 + q0z * q0z * s012 -
            qperpx * qperpx * s012 - qperpz * qperpz * s012 - q0y * q0z * s022 -
            qperpw * qperpx * s022 + qperpy * qperpz * s022 -
            qperpx * qperpy * s102 - qperpw * qperpz * s102 +
            q0x * q0x * (s012 - s112) - q0z * q0z * s112 +
            qperpx * qperpx * s112 + qperpz * qperpz * s112 + q0y * q0z * s122 +
            qperpw * qperpx * s122 - qperpy * qperpz * s122 +
            2 * q0z * qperpw * s002 * theta + 2 * q0y * qperpx * s002 * theta -
            4 * q0z * qperpz * s012 * theta + 2 * q0z * qperpy * s022 * theta +
            2 * q0y * qperpz * s022 * theta +
            q0x * (q0w * s022 + q0y * (-s002 + s102) - q0w * s122 +
                   2 * qperpy * s002 * theta - 4 * qperpx * s012 * theta -
                   2 * qperpw * s022 * theta) +
            q0w * (-(q0z * s002) + q0z * s102 + 2 * qperpz * s002 * theta -
                   2 * qperpx * s022 * theta));

    c3[1] = DerivativeTerm(0.,
                           2 *
                               (-(q0x * qperpy * s000) - q0w * qperpz * s000 +
                                2 * q0x * qperpx * s010 + q0x * qperpw * s020 +
                                q0w * qperpx * s020 + q0x * qperpy * s100 +
                                q0w * qperpz * s100 - 2 * q0x * qperpx * s110 -
                                q0x * qperpw * s120 - q0w * qperpx * s120 +
                                q0z * (2 * qperpz * s010 - qperpy * s020 +
                                       qperpw * (-s000 + s100) -
                                       2 * qperpz * s110 + qperpy * s120) +
                                q0y * (-(qperpx * s000) - qperpz * s020 +
                                       qperpx * s100 + qperpz * s120)) *
                               theta,
                           2 *
                               (-(q0x * qperpy * s001) - q0w * qperpz * s001 +
                                2 * q0x * qperpx * s011 + q0x * qperpw * s021 +
                                q0w * qperpx * s021 + q0x * qperpy * s101 +
                                q0w * qperpz * s101 - 2 * q0x * qperpx * s111 -
                                q0x * qperpw * s121 - q0w * qperpx * s121 +
                                q0z * (2 * qperpz * s011 - qperpy * s021 +
                                       qperpw * (-s001 + s101) -
                                       2 * qperpz * s111 + qperpy * s121) +
                                q0y * (-(qperpx * s001) - qperpz * s021 +
                                       qperpx * s101 + qperpz * s121)) *
                               theta,
                           2 *
                               (-(q0x * qperpy * s002) - q0w * qperpz * s002 +
                                2 * q0x * qperpx * s012 + q0x * qperpw * s022 +
                                q0w * qperpx * s022 + q0x * qperpy * s102 +
                                q0w * qperpz * s102 - 2 * q0x * qperpx * s112 -
                                q0x * qperpw * s122 - q0w * qperpx * s122 +
                                q0z * (2 * qperpz * s012 - qperpy * s022 +
                                       qperpw * (-s002 + s102) -
                                       2 * qperpz * s112 + qperpy * s122) +
                                q0y * (-(qperpx * s002) - qperpz * s022 +
                                       qperpx * s102 + qperpz * s122)) *
                               theta);

    c4[1] = DerivativeTerm(
        0.,
        -(q0x * qperpy * s000) - q0w * qperpz * s000 + 2 * q0x * qperpx * s010 +
            q0x * qperpw * s020 + q0w * qperpx * s020 + q0x * qperpy * s100 +
            q0w * qperpz * s100 - 2 * q0x * qperpx * s110 -
            q0x * qperpw * s120 - q0w * qperpx * s120 +
            2 * qperpx * qperpy * s000 * theta +
            2 * qperpw * qperpz * s000 * theta + 2 * q0x * q0x * s010 * theta +
            2 * q0z * q0z * s010 * theta - 2 * qperpx * qperpx * s010 * theta -
            2 * qperpz * qperpz * s010 * theta + 2 * q0w * q0x * s020 * theta -
            2 * qperpw * qperpx * s020 * theta +
            2 * qperpy * qperpz * s020 * theta +
            q0y * (-(qperpx * s000) - qperpz * s020 + qperpx * s100 +
                   qperpz * s120 - 2 * q0x * s000 * theta) +
            q0z * (2 * qperpz * s010 - qperpy * s020 + qperpw * (-s000 + s100) -
                   2 * qperpz * s110 + qperpy * s120 - 2 * q0w * s000 * theta -
                   2 * q0y * s020 * theta),
        -(q0x * qperpy * s001) - q0w * qperpz * s001 + 2 * q0x * qperpx * s011 +
            q0x * qperpw * s021 + q0w * qperpx * s021 + q0x * qperpy * s101 +
            q0w * qperpz * s101 - 2 * q0x * qperpx * s111 -
            q0x * qperpw * s121 - q0w * qperpx * s121 +
            2 * qperpx * qperpy * s001 * theta +
            2 * qperpw * qperpz * s001 * theta + 2 * q0x * q0x * s011 * theta +
            2 * q0z * q0z * s011 * theta - 2 * qperpx * qperpx * s011 * theta -
            2 * qperpz * qperpz * s011 * theta + 2 * q0w * q0x * s021 * theta -
            2 * qperpw * qperpx * s021 * theta +
            2 * qperpy * qperpz * s021 * theta +
            q0y * (-(qperpx * s001) - qperpz * s021 + qperpx * s101 +
                   qperpz * s121 - 2 * q0x * s001 * theta) +
            q0z * (2 * qperpz * s011 - qperpy * s021 + qperpw * (-s001 + s101) -
                   2 * qperpz * s111 + qperpy * s121 - 2 * q0w * s001 * theta -
                   2 * q0y * s021 * theta),
        -(q0x * qperpy * s002) - q0w * qperpz * s002 + 2 * q0x * qperpx * s012 +
            q0x * qperpw * s022 + q0w * qperpx * s022 + q0x * qperpy * s102 +
            q0w * qperpz * s102 - 2 * q0x * qperpx * s112 -
            q0x * qperpw * s122 - q0w * qperpx * s122 +
            2 * qperpx * qperpy * s002 * theta +
            2 * qperpw * qperpz * s002 * theta + 2 * q0x * q0x * s012 * theta +
            2 * q0z * q0z * s012 * theta - 2 * qperpx * qperpx * s012 * theta -
            2 * qperpz * qperpz * s012 * theta + 2 * q0w * q0x * s022 * theta -
            2 * qperpw * qperpx * s022 * theta +
            2 * qperpy * qperpz * s022 * theta +
            q0y * (-(qperpx * s002) - qperpz * s022 + qperpx * s102 +
                   qperpz * s122 - 2 * q0x * s002 * theta) +
            q0z * (2 * qperpz * s012 - qperpy * s022 + qperpw * (-s002 + s102) -
                   2 * qperpz * s112 + qperpy * s122 - 2 * q0w * s002 * theta -
                   2 * q0y * s022 * theta));

    c5[1] =
        DerivativeTerm(0.,
                       -2 *
                           (qperpx * qperpy * s000 + qperpw * qperpz * s000 +
                            q0z * q0z * s010 - qperpx * qperpx * s010 -
                            qperpz * qperpz * s010 - q0y * q0z * s020 -
                            qperpw * qperpx * s020 + qperpy * qperpz * s020 -
                            qperpx * qperpy * s100 - qperpw * qperpz * s100 +
                            q0w * q0z * (-s000 + s100) +
                            q0x * q0x * (s010 - s110) - q0z * q0z * s110 +
                            qperpx * qperpx * s110 + qperpz * qperpz * s110 +
                            q0x * (q0y * (-s000 + s100) + q0w * (s020 - s120)) +
                            q0y * q0z * s120 + qperpw * qperpx * s120 -
                            qperpy * qperpz * s120) *
                           theta,
                       -2 *
                           (qperpx * qperpy * s001 + qperpw * qperpz * s001 +
                            q0z * q0z * s011 - qperpx * qperpx * s011 -
                            qperpz * qperpz * s011 - q0y * q0z * s021 -
                            qperpw * qperpx * s021 + qperpy * qperpz * s021 -
                            qperpx * qperpy * s101 - qperpw * qperpz * s101 +
                            q0w * q0z * (-s001 + s101) +
                            q0x * q0x * (s011 - s111) - q0z * q0z * s111 +
                            qperpx * qperpx * s111 + qperpz * qperpz * s111 +
                            q0x * (q0y * (-s001 + s101) + q0w * (s021 - s121)) +
                            q0y * q0z * s121 + qperpw * qperpx * s121 -
                            qperpy * qperpz * s121) *
                           theta,
                       -2 *
                           (qperpx * qperpy * s002 + qperpw * qperpz * s002 +
                            q0z * q0z * s012 - qperpx * qperpx * s012 -
                            qperpz * qperpz * s012 - q0y * q0z * s022 -
                            qperpw * qperpx * s022 + qperpy * qperpz * s022 -
                            qperpx * qperpy * s102 - qperpw * qperpz * s102 +
                            q0w * q0z * (-s002 + s102) +
                            q0x * q0x * (s012 - s112) - q0z * q0z * s112 +
                            qperpx * qperpx * s112 + qperpz * qperpz * s112 +
                            q0x * (q0y * (-s002 + s102) + q0w * (s022 - s122)) +
                            q0y * q0z * s122 + qperpw * qperpx * s122 -
                            qperpy * qperpz * s122) *
                           theta);

    c1[2] = DerivativeTerm(
        -t0z + t1z,
        (qperpw * qperpy * s000 - qperpx * qperpz * s000 - q0y * q0z * s010 -
         qperpw * qperpx * s010 - qperpy * qperpz * s010 - s020 +
         q0y * q0y * s020 + qperpx * qperpx * s020 + qperpy * qperpy * s020 -
         qperpw * qperpy * s100 + qperpx * qperpz * s100 +
         q0x * q0z * (-s000 + s100) + q0y * q0z * s110 +
         qperpw * qperpx * s110 + qperpy * qperpz * s110 +
         q0w * (q0y * (s000 - s100) + q0x * (-s010 + s110)) +
         q0x * q0x * (s020 - s120) + s120 - q0y * q0y * s120 -
         qperpx * qperpx * s120 - qperpy * qperpy * s120),
        (qperpw * qperpy * s001 - qperpx * qperpz * s001 - q0y * q0z * s011 -
         qperpw * qperpx * s011 - qperpy * qperpz * s011 - s021 +
         q0y * q0y * s021 + qperpx * qperpx * s021 + qperpy * qperpy * s021 -
         qperpw * qperpy * s101 + qperpx * qperpz * s101 +
         q0x * q0z * (-s001 + s101) + q0y * q0z * s111 +
         qperpw * qperpx * s111 + qperpy * qperpz * s111 +
         q0w * (q0y * (s001 - s101) + q0x * (-s011 + s111)) +
         q0x * q0x * (s021 - s121) + s121 - q0y * q0y * s121 -
         qperpx * qperpx * s121 - qperpy * qperpy * s121),
        (qperpw * qperpy * s002 - qperpx * qperpz * s002 - q0y * q0z * s012 -
         qperpw * qperpx * s012 - qperpy * qperpz * s012 - s022 +
         q0y * q0y * s022 + qperpx * qperpx * s022 + qperpy * qperpy * s022 -
         qperpw * qperpy * s102 + qperpx * qperpz * s102 +
         q0x * q0z * (-s002 + s102) + q0y * q0z * s112 +
         qperpw * qperpx * s112 + qperpy * qperpz * s112 +
         q0w * (q0y * (s002 - s102) + q0x * (-s012 + s112)) +
         q0x * q0x * (s022 - s122) + s122 - q0y * q0y * s122 -
         qperpx * qperpx * s122 - qperpy * qperpy * s122));

    c2[2] = DerivativeTerm(
        0.,
        (q0w * q0y * s000 - q0x * q0z * s000 - qperpw * qperpy * s000 +
         qperpx * qperpz * s000 - q0w * q0x * s010 - q0y * q0z * s010 +
         qperpw * qperpx * s010 + qperpy * qperpz * s010 + q0x * q0x * s020 +
         q0y * q0y * s020 - qperpx * qperpx * s020 - qperpy * qperpy * s020 -
         q0w * q0y * s100 + q0x * q0z * s100 + qperpw * qperpy * s100 -
         qperpx * qperpz * s100 + q0w * q0x * s110 + q0y * q0z * s110 -
         qperpw * qperpx * s110 - qperpy * qperpz * s110 - q0x * q0x * s120 -
         q0y * q0y * s120 + qperpx * qperpx * s120 + qperpy * qperpy * s120 -
         2 * q0y * qperpw * s000 * theta + 2 * q0z * qperpx * s000 * theta -
         2 * q0w * qperpy * s000 * theta + 2 * q0x * qperpz * s000 * theta +
         2 * q0x * qperpw * s010 * theta + 2 * q0w * qperpx * s010 * theta +
         2 * q0z * qperpy * s010 * theta + 2 * q0y * qperpz * s010 * theta -
         4 * q0x * qperpx * s020 * theta - 4 * q0y * qperpy * s020 * theta),
        (q0w * q0y * s001 - q0x * q0z * s001 - qperpw * qperpy * s001 +
         qperpx * qperpz * s001 - q0w * q0x * s011 - q0y * q0z * s011 +
         qperpw * qperpx * s011 + qperpy * qperpz * s011 + q0x * q0x * s021 +
         q0y * q0y * s021 - qperpx * qperpx * s021 - qperpy * qperpy * s021 -
         q0w * q0y * s101 + q0x * q0z * s101 + qperpw * qperpy * s101 -
         qperpx * qperpz * s101 + q0w * q0x * s111 + q0y * q0z * s111 -
         qperpw * qperpx * s111 - qperpy * qperpz * s111 - q0x * q0x * s121 -
         q0y * q0y * s121 + qperpx * qperpx * s121 + qperpy * qperpy * s121 -
         2 * q0y * qperpw * s001 * theta + 2 * q0z * qperpx * s001 * theta -
         2 * q0w * qperpy * s001 * theta + 2 * q0x * qperpz * s001 * theta +
         2 * q0x * qperpw * s011 * theta + 2 * q0w * qperpx * s011 * theta +
         2 * q0z * qperpy * s011 * theta + 2 * q0y * qperpz * s011 * theta -
         4 * q0x * qperpx * s021 * theta - 4 * q0y * qperpy * s021 * theta),
        (q0w * q0y * s002 - q0x * q0z * s002 - qperpw * qperpy * s002 +
         qperpx * qperpz * s002 - q0w * q0x * s012 - q0y * q0z * s012 +
         qperpw * qperpx * s012 + qperpy * qperpz * s012 + q0x * q0x * s022 +
         q0y * q0y * s022 - qperpx * qperpx * s022 - qperpy * qperpy * s022 -
         q0w * q0y * s102 + q0x * q0z * s102 + qperpw * qperpy * s102 -
         qperpx * qperpz * s102 + q0w * q0x * s112 + q0y * q0z * s112 -
         qperpw * qperpx * s112 - qperpy * qperpz * s112 - q0x * q0x * s122 -
         q0y * q0y * s122 + qperpx * qperpx * s122 + qperpy * qperpy * s122 -
         2 * q0y * qperpw * s002 * theta + 2 * q0z * qperpx * s002 * theta -
         2 * q0w * qperpy * s002 * theta + 2 * q0x * qperpz * s002 * theta +
         2 * q0x * qperpw * s012 * theta + 2 * q0w * qperpx * s012 * theta +
         2 * q0z * qperpy * s012 * theta + 2 * q0y * qperpz * s012 * theta -
         4 * q0x * qperpx * s022 * theta - 4 * q0y * qperpy * s022 * theta));

    c3[2] = DerivativeTerm(
        0.,
        -2 *
            (-(q0w * qperpy * s000) + q0x * qperpz * s000 +
             q0x * qperpw * s010 + q0w * qperpx * s010 -
             2 * q0x * qperpx * s020 + q0w * qperpy * s100 -
             q0x * qperpz * s100 - q0x * qperpw * s110 - q0w * qperpx * s110 +
             q0z * (qperpx * s000 + qperpy * s010 - qperpx * s100 -
                    qperpy * s110) +
             2 * q0x * qperpx * s120 +
             q0y *
                 (qperpz * s010 - 2 * qperpy * s020 + qperpw * (-s000 + s100) -
                  qperpz * s110 + 2 * qperpy * s120)) *
            theta,
        -2 *
            (-(q0w * qperpy * s001) + q0x * qperpz * s001 +
             q0x * qperpw * s011 + q0w * qperpx * s011 -
             2 * q0x * qperpx * s021 + q0w * qperpy * s101 -
             q0x * qperpz * s101 - q0x * qperpw * s111 - q0w * qperpx * s111 +
             q0z * (qperpx * s001 + qperpy * s011 - qperpx * s101 -
                    qperpy * s111) +
             2 * q0x * qperpx * s121 +
             q0y *
                 (qperpz * s011 - 2 * qperpy * s021 + qperpw * (-s001 + s101) -
                  qperpz * s111 + 2 * qperpy * s121)) *
            theta,
        -2 *
            (-(q0w * qperpy * s002) + q0x * qperpz * s002 +
             q0x * qperpw * s012 + q0w * qperpx * s012 -
             2 * q0x * qperpx * s022 + q0w * qperpy * s102 -
             q0x * qperpz * s102 - q0x * qperpw * s112 - q0w * qperpx * s112 +
             q0z * (qperpx * s002 + qperpy * s012 - qperpx * s102 -
                    qperpy * s112) +
             2 * q0x * qperpx * s122 +
             q0y *
                 (qperpz * s012 - 2 * qperpy * s022 + qperpw * (-s002 + s102) -
                  qperpz * s112 + 2 * qperpy * s122)) *
            theta);

    c4[2] = DerivativeTerm(
        0.,
        q0w * qperpy * s000 - q0x * qperpz * s000 - q0x * qperpw * s010 -
            q0w * qperpx * s010 + 2 * q0x * qperpx * s020 -
            q0w * qperpy * s100 + q0x * qperpz * s100 + q0x * qperpw * s110 +
            q0w * qperpx * s110 - 2 * q0x * qperpx * s120 -
            2 * qperpw * qperpy * s000 * theta +
            2 * qperpx * qperpz * s000 * theta - 2 * q0w * q0x * s010 * theta +
            2 * qperpw * qperpx * s010 * theta +
            2 * qperpy * qperpz * s010 * theta + 2 * q0x * q0x * s020 * theta +
            2 * q0y * q0y * s020 * theta - 2 * qperpx * qperpx * s020 * theta -
            2 * qperpy * qperpy * s020 * theta +
            q0z * (-(qperpx * s000) - qperpy * s010 + qperpx * s100 +
                   qperpy * s110 - 2 * q0x * s000 * theta) +
            q0y * (-(qperpz * s010) + 2 * qperpy * s020 +
                   qperpw * (s000 - s100) + qperpz * s110 - 2 * qperpy * s120 +
                   2 * q0w * s000 * theta - 2 * q0z * s010 * theta),
        q0w * qperpy * s001 - q0x * qperpz * s001 - q0x * qperpw * s011 -
            q0w * qperpx * s011 + 2 * q0x * qperpx * s021 -
            q0w * qperpy * s101 + q0x * qperpz * s101 + q0x * qperpw * s111 +
            q0w * qperpx * s111 - 2 * q0x * qperpx * s121 -
            2 * qperpw * qperpy * s001 * theta +
            2 * qperpx * qperpz * s001 * theta - 2 * q0w * q0x * s011 * theta +
            2 * qperpw * qperpx * s011 * theta +
            2 * qperpy * qperpz * s011 * theta + 2 * q0x * q0x * s021 * theta +
            2 * q0y * q0y * s021 * theta - 2 * qperpx * qperpx * s021 * theta -
            2 * qperpy * qperpy * s021 * theta +
            q0z * (-(qperpx * s001) - qperpy * s011 + qperpx * s101 +
                   qperpy * s111 - 2 * q0x * s001 * theta) +
            q0y * (-(qperpz * s011) + 2 * qperpy * s021 +
                   qperpw * (s001 - s101) + qperpz * s111 - 2 * qperpy * s121 +
                   2 * q0w * s001 * theta - 2 * q0z * s011 * theta),
        q0w * qperpy * s002 - q0x * qperpz * s002 - q0x * qperpw * s012 -
            q0w * qperpx * s012 + 2 * q0x * qperpx * s022 -
            q0w * qperpy * s102 + q0x * qperpz * s102 + q0x * qperpw * s112 +
            q0w * qperpx * s112 - 2 * q0x * qperpx * s122 -
            2 * qperpw * qperpy * s002 * theta +
            2 * qperpx * qperpz * s002 * theta - 2 * q0w * q0x * s012 * theta +
            2 * qperpw * qperpx * s012 * theta +
            2 * qperpy * qperpz * s012 * theta + 2 * q0x * q0x * s022 * theta +
            2 * q0y * q0y * s022 * theta - 2 * qperpx * qperpx * s022 * theta -
            2 * qperpy * qperpy * s022 * theta +
            q0z * (-(qperpx * s002) - qperpy * s012 + qperpx * s102 +
                   qperpy * s112 - 2 * q0x * s002 * theta) +
            q0y * (-(qperpz * s012) + 2 * qperpy * s022 +
                   qperpw * (s002 - s102) + qperpz * s112 - 2 * qperpy * s122 +
                   2 * q0w * s002 * theta - 2 * q0z * s012 * theta));

    c5[2] =
        DerivativeTerm(0.,
                       2 *
                           (qperpw * qperpy * s000 - qperpx * qperpz * s000 +
                            q0y * q0z * s010 - qperpw * qperpx * s010 -
                            qperpy * qperpz * s010 - q0y * q0y * s020 +
                            qperpx * qperpx * s020 + qperpy * qperpy * s020 +
                            q0x * q0z * (s000 - s100) - qperpw * qperpy * s100 +
                            qperpx * qperpz * s100 +
                            q0w * (q0y * (-s000 + s100) + q0x * (s010 - s110)) -
                            q0y * q0z * s110 + qperpw * qperpx * s110 +
                            qperpy * qperpz * s110 + q0y * q0y * s120 -
                            qperpx * qperpx * s120 - qperpy * qperpy * s120 +
                            q0x * q0x * (-s020 + s120)) *
                           theta,
                       2 *
                           (qperpw * qperpy * s001 - qperpx * qperpz * s001 +
                            q0y * q0z * s011 - qperpw * qperpx * s011 -
                            qperpy * qperpz * s011 - q0y * q0y * s021 +
                            qperpx * qperpx * s021 + qperpy * qperpy * s021 +
                            q0x * q0z * (s001 - s101) - qperpw * qperpy * s101 +
                            qperpx * qperpz * s101 +
                            q0w * (q0y * (-s001 + s101) + q0x * (s011 - s111)) -
                            q0y * q0z * s111 + qperpw * qperpx * s111 +
                            qperpy * qperpz * s111 + q0y * q0y * s121 -
                            qperpx * qperpx * s121 - qperpy * qperpy * s121 +
                            q0x * q0x * (-s021 + s121)) *
                           theta,
                       2 *
                           (qperpw * qperpy * s002 - qperpx * qperpz * s002 +
                            q0y * q0z * s012 - qperpw * qperpx * s012 -
                            qperpy * qperpz * s012 - q0y * q0y * s022 +
                            qperpx * qperpx * s022 + qperpy * qperpy * s022 +
                            q0x * q0z * (s002 - s102) - qperpw * qperpy * s102 +
                            qperpx * qperpz * s102 +
                            q0w * (q0y * (-s002 + s102) + q0x * (s012 - s112)) -
                            q0y * q0z * s112 + qperpw * qperpx * s112 +
                            qperpy * qperpz * s112 + q0y * q0y * s122 -
                            qperpx * qperpx * s122 - qperpy * qperpy * s122 +
                            q0x * q0x * (-s022 + s122)) *
                           theta);
  }
}
void AnimateTransform::decompose(const Mat4x4 &mat, Vector3f *T, Quaternion *R,
                                 Mat4x4 *S) {
  // Extract T
  // ---------
  T->x = mat.val[0][3];
  T->y = mat.val[1][3];
  T->z = mat.val[2][3];
  // Extract R
  // ---------
  Mat4x4 M = mat;  // M = RS
  // No perspective, safe to remove the 4-th dim.
  M.val[0][3] = M.val[1][3] = M.val[2][3] = M.val[3][0] = M.val[3][1] =
      M.val[3][2] = 0.0;
  M.val[3][3] = 1.0;
  // Polar decomposition.
  // M_{i + 1} = 0.5 * (M_i + inv(M_i^T)).
  Float diff;
  int cnt = 0;
  Mat4x4 R0 = M;
  do {
    // Next Matrix.
    Mat4x4 R1;
    Mat4x4 RinvT = mat4x4_inverse(mat4x4_transpose(R0));
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        R1.val[i][j] = 0.5 * (R0.val[i][j] + RinvT.val[i][j]);
      }
    }
    // Norm diff.
    diff = 0.0;
    for (int i = 0; i < 3; i++) {
      Float n = std::abs(R1.val[i][0] - R0.val[i][0]) +
                std::abs(R1.val[i][1] - R0.val[i][1]) +
                std::abs(R1.val[i][2] - R0.val[i][2]);
      diff = diff > n ? diff : n;
    }
    R0 = R1;
  } while (++cnt < 100 && diff > 0.0001);
  *R = Quaternion(R0);
  // Extract S
  // ---------
  *S = mat4x4_multiply(mat4x4_inverse(R0), M);
}
void AnimateTransform::interpolate(Float time, Transform *t) const {
  if (!actually_moving || time <= time_st) {
    *t = trans_st;
    return;
  }
  if (time >= time_ed) {
    *t = trans_ed;
    return;
  }
  Float delta = (time - time_st) / (time_ed - time_st);
  Vector3f iT = (1 - delta) * T[0] + delta * T[1];
  Quaternion iR = sph_lerp(R[0], R[1], delta);
  Mat4x4 iS;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      iS.val[i][j] = lerp(S[0].val[i][j], S[1].val[i][j], delta);
    }
  }
  *t = translate(iT) * iR.to_transform() * Transform(iS);
}
Ray AnimateTransform::operator()(const Ray &r) const {
  if (!actually_moving || r.time <= time_st)
    return trans_st(r);
  else if (r.time >= time_ed)
    return trans_ed(r);
  else {
    Transform t;
    interpolate(r.time, &t);
    return t(r);
  }
}
Vector3f AnimateTransform::operator()(const Vector3f &v, Float time) const {
  if (!actually_moving || time <= time_st)
    return trans_st(v);
  else if (time >= time_ed)
    return trans_ed(v);
  else {
    Transform t;
    interpolate(time, &t);
    return t(v);
  }
}
Point3f AnimateTransform::operator()(const Point3f &p, Float time) const {
  if (!actually_moving || time <= time_st)
    return trans_st(p);
  else if (time >= time_ed)
    return trans_ed(p);
  else {
    Transform t;
    interpolate(time, &t);
    return t(p);
  }
}
Bound3f AnimateTransform::motion_bound(const Bound3f &b) const {
  if (!actually_moving) return trans_st(b);
  if (!has_rotation) return bound_union(trans_st(b), trans_ed(b));
  // Return motion bounds including animated rotation.
  Bound3f bound;
  for (int corner = 0; corner < 8; ++corner)
    bound = bound_union(bound, point_bound(b.corner(corner)));
  return bound;
}
Bound3f AnimateTransform::point_bound(const Point3f &p) const {
  if (!actually_moving) return Bound3f(trans_st(p));
  Bound3f bounds(trans_st(p), trans_ed(p));
  Float cos_theta = dot(R[0], R[1]);
  Float theta = std::acos(clamp(cos_theta, -1, 1));
  for (int c = 0; c < 3; ++c) {
    // Find any motion derivative zeros for the component c.
    Float zeros[8];
    int nZeros = 0;
    interval_find_zeros(c1[c].eval(p), c2[c].eval(p), c3[c].eval(p),
                        c4[c].eval(p), c5[c].eval(p), theta, Interval(0., 1.),
                        zeros, &nZeros);
    ASSERT((unsigned long long)nZeros <= sizeof(zeros) / sizeof(zeros[0]));

    // Expand bounding box for any motion derivative zeros found.
    for (int i = 0; i < nZeros; ++i) {
      Point3f pz = (*this)(p, lerp(time_st, time_ed, zeros[i]));
      bounds = bound_insert(bounds, pz);
    }
  }
  return bounds;
}
Float AnimateTransform::DerivativeTerm::eval(const Point3f &p) const {
  return kc + kx * p.x + ky * p.y + kz * p.z;
}
}  // namespace TRay