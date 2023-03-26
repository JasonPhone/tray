#pragma once
#include "core/geometry/Point.h"

namespace TRay {
/// @brief Mapping a [0, 1)^2 point to 2D disk by concentric mapping.
/// @param u 
/// @return 
Point2f square_to_disk_concentric(const Point2f &u) {
  // Map uniform random numbers to [-1,1]^2.
  Point2f u_offset = 2.0 * u - Vector2f(1, 1);

  // Handle degeneracy at the origin.
  if (u_offset.x == 0 && u_offset.y == 0) return Point2f(0, 0);

  // Concentric map to point.
  Float theta, r;
  if (std::abs(u_offset.x) > std::abs(u_offset.y)) {
    r = u_offset.x;
    theta = PI_DIV_4 * (u_offset.y / u_offset.x);
  } else {
    r = u_offset.y;
    theta = PI_DIV_2 - PI_DIV_4 * (u_offset.x / u_offset.y);
  }
  return r * Point2f(std::cos(theta), std::sin(theta));
}
}  // namespace TRay