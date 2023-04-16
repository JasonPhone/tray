#pragma once
#include "core/Filter.h"
#include "core/geometry/Point.h"

namespace TRay {
/// @brief Take only the center.
class NoFilter : public Filter {
 public:
  NoFilter() : Filter(Vector2f(0.1, 0.1)) {}
  Float evaluate(const Point2f &p) const override {
    return 1;
  }
};
}  // namespace TRay