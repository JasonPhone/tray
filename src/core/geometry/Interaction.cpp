#include "core/geometry/Interaction.h"
#include "core/primitives/Primitive.h"
#include "core/Light.h"

namespace TRay {
SurfaceInteraction::SurfaceInteraction(const Point3f &p, const Point2f &uv,
                                       const Vector3f &wo, const Vector3f &dpdu,
                                       const Vector3f &dpdv, Float time,
                                       const Shape *sh_ptr)
    : Interaction(p, Normal3f(normalize(cross(dpdu, dpdv))), wo, time),
      uv(uv),
      dpdu(dpdu),
      dpdv(dpdv),
      shape(sh_ptr) {
  // Shading data.
  shading.n = n;
  shading.dpdu = dpdu;
  shading.dpdv = dpdv;
}
void SurfaceInteraction::fill_scattering_func(const Ray &ray, bool allow_multi_lobes,
                                              TransportMode mode) {
  primitive->fill_scattering_func(this, mode, allow_multi_lobes);
}
Spectrum SurfaceInteraction::Le(const Vector3f &w) const {
  const AreaLight *light = primitive->area_light();
  return light ? light->L(*this, w) : Spectrum(0.0);
}
}  // namespace TRay