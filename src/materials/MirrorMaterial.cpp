#include "materials/MirrorMaterial.h"

#include "core/Texture.h"
#include "core/geometry/Interaction.h"
#include "core/reflection/BSDF.h"
#include "core/reflection/FresnelSpecular.h"


namespace TRay {

void MirrorMaterial::fill_scattering_func(SurfaceInteraction *si, TransportMode,
                                          bool) const {
  si->bsdf = new BSDF(*si);
  Spectrum r = m_color->evaluate(*si).clamp();
  FresnelConst *fresnel = new FresnelConst;
  SpecularReflection *bxdf = new SpecularReflection(r, fresnel);
  si->bsdf->add_BxDF(*bxdf);
}
}  // namespace TRay