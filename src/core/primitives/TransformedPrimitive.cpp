#include "core/primitives/TransformedPrimitive.h"
#include "core/geometry/Shape.h"

namespace TRay {

Bound3f TransformedPrimitive::do_world_bound() const {
  return m_prim_to_world.motion_bound(m_primitive->world_bound());
}
/**
 * @brief The ray will be transformed into primitive's space and passed to its
 * intersect() function.
 *
 */
bool TransformedPrimitive::do_intersect(const Ray &ray,
                                        SurfaceInteraction *si) const {
  // Transform ray to primitive space.
  Transform prim_to_world_erp;
  m_prim_to_world.interpolate(ray.time, &prim_to_world_erp);
  Ray t_ray = prim_to_world_erp.inverse()(ray);
  // Intersection test.
  if (!m_primitive->intersect(t_ray, si)) return false;
  ray.t_max = t_ray.t_max;
  // Transform back world space.
  if (!prim_to_world_erp.is_identity()) *si = prim_to_world_erp(*si);
  return true;
}
bool TransformedPrimitive::do_intersect_test(const Ray &ray) const {
  Transform prim_to_world_erp;
  m_prim_to_world.interpolate(ray.time, &prim_to_world_erp);
  // Inverse then interpolate does not guarantee the same result due to polar
  // decomposition.
  Transform world_to_prim_erp = prim_to_world_erp.inverse();
  return m_primitive->intersect_test(world_to_prim_erp(ray));
}
AreaLight *TransformedPrimitive::do_area_light() const {
  SError(
      "TransformedPrimitive::do_area_light: Transformed primitive "
      "has no light source!");
  ASSERT(0);
  return nullptr;
}
Material *TransformedPrimitive::do_material() const {
  SError(
      "TransformedPrimitive::do_material: Transformed primitive "
      "has no material!");
  ASSERT(0);
  return nullptr;
}
void TransformedPrimitive::do_fill_scattering_func(
    SurfaceInteraction *si, TransportMode mode, bool allow_multi_lobes) const {
  SError(
      "TransformedPrimitive::do_fill_scattering_func: Transformed primitive "
      "has no BxDF!");
  ASSERT(0);
}

}  // namespace TRay