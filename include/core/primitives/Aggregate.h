#pragma once
#include "core/TRay.h"
#include "core/primitives/Primitive.h"

/**
 * The group of multiple Primitives, as a Primitive itself.
 * Hide the scene details against the Integrator.
 */

namespace TRay {
/// @brief Other virtuals are to be implemented by concrete acceleration structures.
class Aggregate : public Primitive {
 private:
  AreaLight *do_area_light() const override;
  Material *do_material() const override;
  void do_fill_scattering_func(SurfaceInteraction *si, TransportMode mode,
                               bool allow_multi_lobes) const override;
};

}  // namespace TRay