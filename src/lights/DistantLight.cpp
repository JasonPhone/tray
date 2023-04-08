#include "lights/DistantLight.h"
#include "core/Scene.h"

namespace TRay {
void DistantLight::preprocess(const Scene &scene) {
  scene.world_bound().bounding_sphere(&m_world_center, &m_world_radius);
}

Spectrum DistantLight::sample_Li(const Interaction &ref, const Point2f &u,
                                 Vector3f *wi, Float *pdf_value,
                                 VisibilityTester *vis) const {
  *wi = m_w_light;
  *pdf_value = 1.0;
  // Make sure the point is outside of scene.
  Point3f p_outside = ref.p + 2 * m_world_radius * m_w_light;
  *vis = VisibilityTester(ref, Interaction(p_outside, ref.time));
  return m_L;
}
Spectrum DistantLight::total_power() const {
  return m_L * PI * m_world_radius * m_world_radius;
}

}  // namespace TRay