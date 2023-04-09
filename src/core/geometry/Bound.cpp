#include "core/geometry/Bound.h"
#include "core/geometry/Point.h"
#include "core/geometry/Ray.h"
namespace TRay {
Bound2iIterator &Bound2iIterator::operator++() {
  increase();
  return *this;
}
/// @brief Suffix increase, itr++.
Bound2iIterator Bound2iIterator::operator++(int) {
  Bound2iIterator old = *this;
  ++*this;
  return old;
}
bool Bound2iIterator::operator==(Bound2iIterator other) const {
  return pos.y == other.pos.y && bound == other.bound;
}
bool Bound2iIterator::operator!=(Bound2iIterator other) const {
  return !(*this == other);
}
Point2i Bound2iIterator::operator*() const { return pos; }
Bound2iIterator Bound2iIterator::begin() const {
  return Bound2iIterator(bound, bound.p_min);
}
Bound2iIterator Bound2iIterator::end() const {
  return Bound2iIterator(bound, bound.p_max);
}

void Bound2iIterator::increase() {
  ++pos.x;
  if (pos.x == bound.p_max.x) {
    pos.x = bound.p_min.x;
    ++pos.y;
  }
}

}  // namespace TRay