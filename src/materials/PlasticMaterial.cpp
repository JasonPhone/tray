#include "materials/PlasticMaterial.h"
#include "core/reflection/BSDF.h"

namespace TRay {
void PlasticMaterial::fill_scattering_func(SurfaceInteraction *si,
                                           TransportMode mode,
                                           bool allow_multi_lobes) const {
  si->bsdf = new BSDF(*si);
}
}  // namespace TRay