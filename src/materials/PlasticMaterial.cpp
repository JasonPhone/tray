#include "materials/PlasticMaterial.h"
#include "core/reflection/BSDF.h"

namespace TRay {
void PlasticMaterial::fill_scattering_func(SurfaceInteraction *si,
                                           TransportMode ,
                                           bool ) const {
  si->bsdf = new BSDF(*si);
}
}  // namespace TRay