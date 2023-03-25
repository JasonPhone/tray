#include "core/primitives/Aggregate.h"
#include "core/geometry/Transform.h"

namespace TRay {
AreaLight *Aggregate::area_light() const {
  SError("Aggregate::area_light: Aggregate has no light source!");
  ASSERT(0);
  return nullptr;
}
Material *Aggregate::material() const {
  SError("Aggregate::material: Aggregate has no material!");
  ASSERT(0);
  return nullptr;
}
void Aggregate::fill_scattering_func(SurfaceInteraction *si, TransportMode mode,
                                     bool allow_multi_lobes) const {
  SError("Aggregate::fill_scattering_func: Aggregate has no BxDF!");
  ASSERT(0);
}
}  // namespace TRay