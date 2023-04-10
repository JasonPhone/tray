#pragma once
#include "core/TRay.h"
#include "core/geometry/Bound.h"
#include "core/Light.h"
#include "core/primitives/primitives.h"

namespace TRay {
class Scene {
 public:
  Scene(std::shared_ptr<Primitive> aggregate,
        const std::vector<std::shared_ptr<Light>> &lights)
      : m_aggregate(aggregate), m_lights(lights) {
    m_world_bound = aggregate->world_bound();
    for (const auto &light : lights) {
      light->preprocess(*this);
      if (light->m_type & LIGHT_INFINITE) m_infinite_lights.push_back(light);
    }
  }
  const Bound3f &world_bound() const { return m_world_bound; }
  bool intersect(const Ray &ray, SurfaceInteraction *isect) const;
  bool intersect_test(const Ray &ray) const;

  std::vector<std::shared_ptr<Light>> m_lights;
  // Store separately in case we only want to loop over infinite ones.
  std::vector<std::shared_ptr<Light>> m_infinite_lights;

 private:
  std::shared_ptr<Primitive> m_aggregate;
  Bound3f m_world_bound;
};
}  // namespace TRay