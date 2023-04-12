#include "materials/MirrorMaterial.h"
#include "core/geometry/Interaction.h"
#include "core/reflection/BSDF.h"
#include "core/reflection/FresnelSpecular.h"

namespace TRay {

void MirrorMaterial::fill_scattering_func(SurfaceInteraction *si,
                                          TransportMode mode,
                                          bool allow_multi_lobes) const {
  si->bsdf = new BSDF(*si);
  FresnelConst *fresnel = new FresnelConst; 
  SpecularReflection *bxdf = new SpecularReflection(Spectrum(1.0), fresnel);
  si->bsdf->add_BxDF(*bxdf);
}
}  // namespace TRay