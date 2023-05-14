#include "materials/MatteMaterial.h"

#include "core/Texture.h"
#include "core/reflection/BSDF.h"
#include "core/reflection/Lambertian.h"
#include "core/reflection/OrenNayar.h"


namespace TRay {
void MatteMaterial::fill_scattering_func(SurfaceInteraction *si, TransportMode,
                                         bool) const {
  si->bsdf = new BSDF(*si);
  Spectrum r = m_diffuse->evaluate(*si).clamp();
  Float sig = clamp(m_sigma->evaluate(*si), 0, 90);
  if (!r.is_black()) {
    if (sig == 0) {
      // Fill a Lambertian.
      LambertianReflection *bxdf = new LambertianReflection(r);
      si->bsdf->add_BxDF(*bxdf);
    } else {
      // Fill an OrenNayar.
      OrenNayar *bxdf = new OrenNayar(r, sig);
      si->bsdf->add_BxDF(*bxdf);
    }
  }
}
}  // namespace TRay