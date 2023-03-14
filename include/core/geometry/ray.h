/**
 * @file ray.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Class Ray and RayDifferential(not needed by now).
 * @version 0.1
 * @date 2023-03-14
 */
#pragma once
#include "core/tray.h"
#include "core/geometry/vector.h"
#include "core/geometry/point.h"

/**
 * @brief Ray is not a template class because we need a high precision.
 *
 */
namespace TRay {
class Ray {
 public:
  Ray(const Point3f &o, const Vector3f &d, Float t = 0.0,
      Float t_maxv = FLOAT_INF)
      : ori(o), dir(d), t_max(t_maxv), time(t) {}
  Ray() : t_max(FLOAT_INF), time(0.0) {}
  Point3f ori;
  Vector3f dir;
  Float time;
  /// @brief We need to restrict the extent of a Ray when testing intersection.
  mutable Float t_max;
  /// @brief Get the position of Ray by given time.
  Point3f operator()(Float t) {
    return ori + t * dir;
  }
};

}  // namespace TRay