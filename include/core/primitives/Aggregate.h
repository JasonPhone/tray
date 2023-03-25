#pragma once
#include "core/TRay.h"
#include "core/primitives/Primitive.h"

/**
 * The group of multiple Primitives, as a Primitive itself.
 * Hide the scene details against the Integrator.
 */

namespace TRay {
class Aggregate : public Primitive {
  AreaLight *area_light() const override;
  Material *material() const override;
  void fill_scattering_func(SurfaceInteraction *si, TransportMode mode,
                               bool allow_multi_lobes) const override;
  /// @brief Other virtuals are to be implemented by concrete structures:
  // Bound3f world_bound() const override;
  // bool intersect(const Ray &ray, SurfaceInteraction *si) const override;
  // bool intersect_test(const Ray &ray) const override;
 private:
};

}  // namespace TRay