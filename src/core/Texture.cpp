#include "core/Texture.h"
#include "core/geometry/Point.h"
#include "core/geometry/Interaction.h"

namespace TRay {
UVMapping2D::UVMapping2D(Float su, Float sv, Float offset_u, Float offset_v)
    : m_su(su), m_sv(sv), m_offset_u(offset_u), m_offset_v(offset_v) {}

Point2f UVMapping2D::map(const SurfaceInteraction &si) const {
  // Scale and shift.
  return Point2f(m_su * si.uv.x + m_offset_u, m_sv * si.uv.y + m_offset_v);
}

SphericalMapping2D::SphericalMapping2D(const Transform &world_to_texture)
    : m_world_to_tex(world_to_texture) {}
Point2f SphericalMapping2D::map(const SurfaceInteraction &si) const {
  Point2f st = onto_sphere(si.p);
  return st;
}
Point2f SphericalMapping2D::onto_sphere(const Point3f &p) const {
  Vector3f v = normalize(m_world_to_tex(p) - Point3f(0, 0, 0));
  Float theta = spherical_theta_of(v);
  Float phi = spherical_phi_of(v);
  return Point2f(theta * PI_INV, phi * PI_INV2);
}

PlanarMapping2D::PlanarMapping2D(const Vector3f &s, const Vector3f &t,
                                 Float offset_s, Float offset_t)
    : m_vs(s), m_vt(t), m_offset_s(offset_s), m_offset_t(offset_t) {}
Point2f PlanarMapping2D::map(const SurfaceInteraction &si) const {
  Vector3f v(si.p);
  return Point2f(dot(v, m_vs) + m_offset_s, dot(v, m_vt) + m_offset_t);
}

TransformMapping3D::TransformMapping3D(const Transform &world_to_texture)
    : m_world_to_tex(world_to_texture) {}
Point3f TransformMapping3D::map(const SurfaceInteraction &si) const {
  return m_world_to_tex(si.p);
}


}  // namespace TRay