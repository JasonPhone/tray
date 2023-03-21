#pragma once
#include "core/primitives/Aggregate.h"

namespace TRay {
class LinearAccel : public Aggregate {
  /// @brief LinearAccel cstr.
  /// @param primitives Vector of primitives.
  /// @note Move sematic is used, @param primitives will lost.
  LinearAccel(std::vector<std::shared_ptr<Primitive>> primitives);

 private:
  std::vector<std::shared_ptr<Primitive>> m_primitives;
  Bound3f m_world_bound;

  Bound3f do_world_bound() const override;
  bool do_intersect(const Ray &ray, SurfaceInteraction *si) const override;
  bool do_intersect_test(const Ray &ray) const override;
};

}  // namespace TRay