#include "accelerators/LinearAccel.h"

namespace TRay {

LinearAccel::LinearAccel(std::vector<std::shared_ptr<Primitive>> primitives)
    : m_primitives(std::move(primitives)) {
  SInfo("LinearAccel:: Created an accelerator with" +
        string_format("\n\t%d primitives.", (int)m_primitives.size()));
  for (const auto &prim : m_primitives) {
    m_world_bound = bound_union(m_world_bound, prim->world_bound());
  }
}
Bound3f LinearAccel::world_bound() const { return m_world_bound; }
bool LinearAccel::intersect(const Ray &ray, SurfaceInteraction *si) const {
  bool hitted = false;
  for (const auto &prim : m_primitives)
    if (prim->world_bound().intersect_test(ray, nullptr, nullptr) &&
        prim->intersect(ray, si))
      hitted = true;
  return hitted;
}
bool LinearAccel::intersect_test(const Ray &ray) const {
  for (const auto &prim : m_primitives) {
    if (prim->world_bound().intersect_test(ray, nullptr, nullptr) &&
        prim->intersect_test(ray))
      return true;
  }
  return false;
}
}  // namespace TRay