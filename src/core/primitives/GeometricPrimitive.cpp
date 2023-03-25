#include "core/primitives/GeometricPrimitive.h"
#include "core/geometry/Shape.h"

namespace TRay {

Bound3f GeometricPrimitive::world_bound() const {
  return m_shape->world_bound();
}
bool GeometricPrimitive::intersect(const Ray &ray,
                                   SurfaceInteraction *si) const {
  Float t_hit;
  if (!m_shape->intersect(ray, &t_hit, si)) return false;
  ray.t_max = t_hit;
  si->primitive = this;
  return true;
}
bool GeometricPrimitive::intersect_test(const Ray &ray) const {
  return m_shape->intersect_test(ray);
}
AreaLight *GeometricPrimitive::area_light() const { return m_area_light.get(); }
Material *GeometricPrimitive::material() const { return m_material.get(); }
void GeometricPrimitive::fill_scattering_func(SurfaceInteraction *si,
                                              TransportMode mode,
                                              bool allow_multi_lobes) const {
  SError(
      "GeometricPrimitive::fill_scattering_func: Class Material is not "
      "implemented yet!");
  ASSERT(0);
  // if (m_material) m_material->fill_scattering_func(si, mode,
  // allow_multi_lobes);
}

}  // namespace TRay