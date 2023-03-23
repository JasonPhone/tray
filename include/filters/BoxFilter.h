#pragma once
#include "core/Filter.h"

namespace TRay {
/// @brief Returns a constant value (1.0) for any sample point.
class BoxFilter : public Filter {
 public:
  BoxFilter(const Vector2f &radius) : Filter(radius) {}
  Float evaluate(const Point2f &p) const override;
};
}  // namespace TRay