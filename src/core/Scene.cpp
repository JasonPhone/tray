#include "core/Scene.h"

namespace TRay {

Scene::Scene(std::shared_ptr<Primitive> aggregate,
             const std::vector<std::shared_ptr<Light>> &lights)
    : m_lights(lights), m_aggregate(aggregate) {
  SInfo("Scene:: Created scene with" +
        string_format("\n\t%d lights", (int)lights.size()));
  m_world_bound = aggregate->world_bound();
  for (const auto &light : lights) {
    light->preprocess(*this);
    if (light->m_type & LIGHT_INFINITE) {
      SInfo("Scene::Scene: An infinite light.");
      m_infinite_lights.push_back(light);
    }
  }
}
bool Scene::intersect(const Ray &ray, SurfaceInteraction *si) const {
  ASSERT(ray.dir != Vector3f(0, 0, 0));
  return m_aggregate->intersect(ray, si);
}

bool Scene::intersect_test(const Ray &ray) const {
  ASSERT(ray.dir != Vector3f(0, 0, 0));
  return m_aggregate->intersect_test(ray);
}

}  // namespace TRay