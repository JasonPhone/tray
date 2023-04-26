#include "core/geometry/Interaction.h"
#include "core/primitives/Primitive.h"
#include "core/geometry/Shape.h"
#include "core/Light.h"

namespace TRay {
SurfaceInteraction::SurfaceInteraction(const Point3f &p, const Vector3f &err,
                                       const Point2f &uv, const Vector3f &wo,
                                       const Vector3f &dpdu,
                                       const Vector3f &dpdv, Float time,
                                       const Shape *sh_ptr)
    : Interaction(p, Normal3f(normalize(cross(dpdu, dpdv))), err, wo, time),
      uv(uv),
      dpdu(dpdu),
      dpdv(dpdv),
      shape(sh_ptr) {
  // Shading data.
  shading.n = n;
  shading.dpdu = dpdu;
  shading.dpdv = dpdv;
  // The intersection is done in left-handed space, adjust if needed.
  if (shape && (shape->flip_normal ^ shape->swap_handness)) {
    n *= -1;
    shading.n *= -1;
  }
}
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
  // The intersection is done in left-handed space, adjust if needed.
  if (shape && (shape->flip_normal ^ shape->swap_handness)) {
    n *= -1;
    shading.n *= -1;
  }
}
void SurfaceInteraction::fill_scattering_func(const Ray &ray,
                                              TransportMode mode,
                                              bool allow_multi_lobes) {
  primitive->fill_scattering_func(this, mode, allow_multi_lobes);
}

void SurfaceInteraction::set_shading_geometry(const Vector3f sdpdu,
                                              const Vector3f sdpdv,
                                              bool is_robust) {
  shading.n = Normal3f(normalize(cross(sdpdu, sdpdv)));
  if (is_robust)
    n = align_with(n, shading.n);
  else
    shading.n = align_with(shading.n, n);
  shading.dpdu = sdpdu;
  shading.dpdv = sdpdv;
}
Spectrum SurfaceInteraction::Le(const Vector3f &w) const {
  const AreaLight *light = primitive->area_light();
  // if (light) {
  //   // SDebug("this primitive has an area light");
  //   return light->L(*this, w);
  // } else {
  //   // SDebug("this primitive does not have area light");
  //   return Spectrum(0.0);
  // }
  return light ? light->L(*this, w) : Spectrum(0.0);
}
}  // namespace TRay