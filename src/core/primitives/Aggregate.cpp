#include "core/primitives/Aggregate.h"
#include "core/geometry/Transform.h"

namespace TRay {
AreaLight *Aggregate::do_area_light() const {
  SError("Aggregate::do_area_light: Aggregate has no light source!");
  ASSERT(0);
  return nullptr;
}
Material *Aggregate::do_material() const {
  SError("Aggregate::do_material: Aggregate has no material!");
  ASSERT(0);
  return nullptr;
}
void Aggregate::do_fill_scattering_func(SurfaceInteraction *si,
                                        TransportMode mode,
                                        bool allow_multi_lobes) const {
  SError("Aggregate::do_fill_scattering_func: Aggregate has no BxDF!");
  ASSERT(0);
}
}  // namespace TRay