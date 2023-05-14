#include "filters/BoxFilter.h"

namespace TRay {
Float BoxFilter::evaluate(const Point2f &) const {
  return 1.0;
}
} // namespace TRay