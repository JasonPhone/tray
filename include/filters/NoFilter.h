#pragma once
#include "core/Filter.h"
#include "core/geometry/Point.h"

namespace TRay {
/// @brief Take only the samples with relative distance
///        less than 0.3 in both axis.
class NoFilter : public Filter {
 public:
  NoFilter() : Filter(Vector2f(1, 1)) {}
  Float evaluate(const Point2f &p) const override {
    if (std::abs(p.x) < 0.5 && std::abs(p.y) < 0.5) return 1;
    return 0;
  }
};
}  // namespace TRay