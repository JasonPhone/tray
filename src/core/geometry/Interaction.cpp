#include "core/geometry/Interaction.h"

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
}  // namespace TRay