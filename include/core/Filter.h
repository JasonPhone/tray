#pragma once
#include "core/TRay.h"
#include "core/geometry/Vector.h"

namespace TRay {
/// @brief Returns weight of a sample point.
class Filter {
 public:
  virtual ~Filter() {}
  Filter(const Vector2f &radius)
      : m_radius(radius), m_radius_inv(1.0 / radius.x, 1.0 / radius.y) {
    ASSERT(!std::isinf(m_radius.x) && !std::isinf(m_radius.y));
    ASSERT(!std::isinf(m_radius_inv.x) && !std::isinf(m_radius_inv.y));
  }
  /// @brief Get the filter weight by the RELATIVE distance.
  virtual Float evaluate(const Point2f &p) const = 0;
  const Vector2f m_radius, m_radius_inv;
};
}  // namespace TRay