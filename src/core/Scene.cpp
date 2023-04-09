#include "core/Scene.h"

namespace TRay {
bool Scene::intersect(const Ray &ray, SurfaceInteraction *isect) const {
  ASSERT(ray.dir != Vector3f(0, 0, 0));
  return m_aggregate->intersect(ray, isect);
}

bool Scene::intersect_test(const Ray &ray) const {
  ASSERT(ray.dir != Vector3f(0, 0, 0));
  return m_aggregate->intersect_test(ray);
}

}  // namespace TRay