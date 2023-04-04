#include "core/Light.h"

namespace TRay {
Light::Light(LightType type, const Transform &light_to_world, int n_samples = 1)
    : m_type(type), m_n_samples(n_samples), m_light_to_world(light_to_world) {
  // Warn for scaling.
  if (m_light_to_world.has_scale())
    SWarn(
        "Light::Light: This light source has been scaled, the area is "
        "inconsistent!");
}

bool VisibilityTester::blocked(const Scene &scene) const {
  return scene.intersect_test(m_ref0.ray_to(m_ref1));
}
}  // namespace TRay