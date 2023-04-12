#include "core/Light.h"
#include "core/Scene.h"

namespace TRay {
Light::Light(LightType type, const Transform &light_to_world, int n_samples)
    : m_type(type), m_n_samples(n_samples), m_light_to_world(light_to_world) {
  // Warn for scaling.
  if (m_light_to_world.has_scale())
    SWarn(
        "Light::Light: This light source has been scaled, the area is "
        "inconsistent!");
}

bool VisibilityTester::blocked(const Scene &scene) const {
  // SDebug("vis test from " + m_ref0.p.to_string() + " to " + m_ref1.p.to_string());
  return scene.intersect_test(m_ref0.ray_to(m_ref1));
}
}  // namespace TRay