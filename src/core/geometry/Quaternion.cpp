#include "core/geometry/Quaternion.h"
#include "core/geometry/Vector.h"
#include "core/geometry/Transform.h"

namespace TRay {
// Quaternion impl.
// ----------------
/// @brief Shoemake, K. 1991. Quaternions and 4x4 matrices. Graphics Gems II.
Quaternion::Quaternion(const Transform &t) {
  const Mat4x4 &m = t.m;
  Float trace = m.val[0][0] + m.val[1][1] + m.val[2][2];
  if (trace > 0.f) {
    // Compute w from matrix trace, then xyz
    // 4w^2 = val[0][0] + val[1][1] + val[2][2] + val[3][3] (but val[3][3] == 1)
    Float s = std::sqrt(trace + 1.0f);
    real = s / 2.0f;
    s = 0.5f / s;
    imag.x = (m.val[2][1] - m.val[1][2]) * s;
    imag.y = (m.val[0][2] - m.val[2][0]) * s;
    imag.z = (m.val[1][0] - m.val[0][1]) * s;
  } else {
    // Compute largest of x, y, or z, then remaining components
    const int nxt[3] = {1, 2, 0};
    Float q[3];
    int i = 0;
    if (m.val[1][1] > m.val[0][0]) i = 1;
    if (m.val[2][2] > m.val[i][i]) i = 2;
    int j = nxt[i];
    int k = nxt[j];
    Float s = std::sqrt((m.val[i][i] - (m.val[j][j] + m.val[k][k])) + 1.0f);
    q[i] = s * 0.5f;
    if (s != 0.f) s = 0.5f / s;
    real = (m.val[k][j] - m.val[j][k]) * s;
    q[j] = (m.val[j][i] + m.val[i][j]) * s;
    q[k] = (m.val[k][i] + m.val[i][k]) * s;
    imag.x = q[0];
    imag.y = q[1];
    imag.z = q[2];
  }
}
Quaternion &Quaternion::operator+=(const Quaternion &other) {
  imag += other.imag;
  real += other.real;
  return *this;
}
Quaternion Quaternion::operator+(const Quaternion &other) const {
  Quaternion ret = *this;
  ret += other;
  return ret;
}
Quaternion &Quaternion::operator-=(const Quaternion &other) {
  imag -= other.imag;
  real -= other.real;
  return *this;
}
Quaternion Quaternion::operator-() const {
  Quaternion ret;
  ret.imag = -imag;
  ret.real = -real;
  return ret;
}
Quaternion Quaternion::operator-(const Quaternion &other) const {
  Quaternion ret = *this;
  ret -= other;
  return ret;
}
Quaternion &Quaternion::operator*=(Float s) {
  imag *= s;
  real *= s;
  return *this;
}
Quaternion Quaternion::operator*(Float s) const {
  Quaternion ret = *this;
  ret.imag *= s;
  ret.real *= s;
  return ret;
}
Quaternion operator*(Float s, const Quaternion &q) { return q * s; }

inline Float dot(const Quaternion &q1, const Quaternion &q2) {
  return dot(q1.imag, q2.imag) + q1.real * q2.real;
}
Transform Quaternion::to_transform() const {
  Float xx = imag.x * imag.x, yy = imag.y * imag.y, zz = imag.z * imag.z;
  Float xy = imag.x * imag.y, xz = imag.x * imag.z, yz = imag.y * imag.z;
  Float wx = imag.x * real, wy = imag.y * real, wz = imag.z * real;

  Mat4x4 m;
  m.val[0][0] = 1 - 2 * (yy + zz);
  m.val[0][1] = 2 * (xy + wz);
  m.val[0][2] = 2 * (xz - wy);
  m.val[1][0] = 2 * (xy - wz);
  m.val[1][1] = 1 - 2 * (xx + zz);
  m.val[1][2] = 2 * (yz + wx);
  m.val[2][0] = 2 * (xz + wy);
  m.val[2][1] = 2 * (yz - wx);
  m.val[2][2] = 1 - 2 * (xx + yy);

  // Left-handed.
  return Transform(mat4x4_transpose(m), m);
}
inline Quaternion normalize(const Quaternion &q) {
  return q * (1.0 / std::sqrt(dot(q, q)));
}
inline Quaternion sph_lerp(const Quaternion &q1, const Quaternion &q2,
                           Float t) {
  Float cos_theta = dot(q1, q2);
  if (cos_theta > .9995)
    return normalize((1 - t) * q1 + t * q2);
  else {
    Float theta = std::acos(clamp(cos_theta, -1, 1));
    Float thetap = theta * t;
    Quaternion qperp = normalize(q2 - q1 * cos_theta);
    return q1 * std::cos(thetap) + qperp * std::sin(thetap);
  }
}
}  // namespace TRay