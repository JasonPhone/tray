#pragma once
#include "core/primitives/Aggregate.h"

namespace TRay {
class LinearAccel : public Aggregate {
 public:
  /// @brief LinearAccel cstr.
  /// @param primitives Vector of primitives.
  /// @note Move sematic is used, @param primitives will lost.
  LinearAccel(std::vector<std::shared_ptr<Primitive>> primitives);
  Bound3f world_bound() const override;
  /// @brief Intersect with all primitives in O(n) time.
  bool intersect(const Ray &ray, SurfaceInteraction *si) const override;
  /// @brief Test intersect with all primitives in O(n) time.
  bool intersect_test(const Ray &ray) const override;

 private:
  std::vector<std::shared_ptr<Primitive>> m_primitives;
  Bound3f m_world_bound;
};

}  // namespace TRay