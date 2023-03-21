#include "accelerators/LinearAccel.h"

namespace TRay {

LinearAccel::LinearAccel(std::vector<std::shared_ptr<Primitive>> primitives)
    : m_primitives(std::move(primitives)) {
  for (const auto &prim : m_primitives) {
    m_world_bound = bound_union(m_world_bound, prim->world_bound());
  }
}
Bound3f LinearAccel::do_world_bound() const { return m_world_bound; }
bool LinearAccel::do_intersect(const Ray &ray, SurfaceInteraction *si) const {
  bool hitted = false;
  for (const auto &prim : m_primitives)
    if (prim->intersect(ray, si)) hitted = true;
  return hitted;
}
bool LinearAccel::do_intersect_test(const Ray &ray) const {
  for (const auto &prim : m_primitives)
    if (prim->intersect_test(ray)) return true;
  return false;
}
}  // namespace TRay