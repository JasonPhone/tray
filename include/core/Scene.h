#pragma once
#include "core/TRay.h"
#include "core/geometry/Bound.h"
#include "core/Light.h"
#include "core/primitives/primitives.h"

namespace TRay {
class Scene {
 public:
  Scene(std::shared_ptr<Primitive> aggregate,
        const std::vector<std::shared_ptr<Light>> &lights);
  const Bound3f &world_bound() const { return m_world_bound; }
  /// @brief Shoot the ray and find intersect with the aggregate member.
  /// @param si Store the surface interaction.
  bool intersect(const Ray &ray, SurfaceInteraction *si) const;
  bool intersect_test(const Ray &ray) const;

  std::vector<std::shared_ptr<Light>> m_lights;
  // Store separately in case we only want to loop over infinite ones.
  std::vector<std::shared_ptr<Light>> m_infinite_lights;

 private:
  std::shared_ptr<Primitive> m_aggregate;
  Bound3f m_world_bound;
};
}  // namespace TRay