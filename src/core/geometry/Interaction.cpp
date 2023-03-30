#include "core/geometry/Interaction.h"
#include "core/primitives/Primitive.h"

namespace TRay {
SurfaceInteraction::SurfaceInteraction(const Point3f &p, const Point2f &uv,
                                       const Vector3f &wo, const Vector3f &dpdu,
                                       const Vector3f &dpdv,
                                       Float time,
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
void SurfaceInteraction::fill_scattering_func(bool allow_multi_lobes, TransportMode mode) {
  primitive->fill_scattering_func(this, mode, allow_multi_lobes);
}
}  // namespace TRay