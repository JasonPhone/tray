/**
 * @file ray.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Class Ray and RayDifferential(not needed by now).
 * @version 0.1
 * @date 2023-03-14
 */
#pragma once
#include "core/TRay.h"
#include "core/geometry/Vector.h"
#include "core/geometry/Point.h"

/// @brief Ray is not a template class because we need a high precision.
namespace TRay {
class Ray {
 public:
  Ray(const Point3f &o, const Vector3f &d, Float t = 0.0,
      Float t_maxv = TRAY_INF)
      : ori(o), dir(d), time(t), t_max(t_maxv) {}
  Ray() : time(0.0), t_max(TRAY_INF) {}
  /// @brief Get the position of Ray by given time.
  Point3f operator()(Float t) const;
  std::string to_string() const {
    return "ray o: " + ori.to_string() + " d: " + dir.to_string();
  }

  Point3f ori;
  Vector3f dir;
  Float time;
  /// @brief We need to restrict the extent of a Ray when testing intersection.
  mutable Float t_max;
};

}  // namespace TRay