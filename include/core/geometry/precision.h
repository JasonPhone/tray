#pragma once
#include "core/TRay.h"
#include "core/geometry/Point.h"
#include "core/geometry/Vector.h"
#include "core/geometry/Normal.h"

namespace TRay {
inline Point3f offset_ray_ori(const Point3f &p, const Vector3f &pError,
                               const Normal3f &n, const Vector3f &w) {
  Float d = dot(abs(n), pError);
  Vector3f offset = d * Vector3f(n);
  if (dot(w, n) < 0) offset = -offset;
  Point3f po = p + offset;
  // Round offset point _po_ away from _p_
  for (int i = 0; i < 3; ++i) {
    if (offset[i] > 0)
      po[i] = next_float_up(po[i]);
    else if (offset[i] < 0)
      po[i] = next_float_down(po[i]);
  }
  return po;
}
}  // namespace TRay