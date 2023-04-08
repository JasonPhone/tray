#include "lights/DiffuseAreaLight.h"

namespace TRay {

Spectrum DiffuseAreaLight::L(const Interaction &inter,
                             const Vector3f &w) const {
  return dot(inter.n, w) > 0.0 ? m_L_emit : Spectrum(0.0);
}
Spectrum DiffuseAreaLight::sample_Li(const Interaction &ref, const Point2f &u,
                                     Vector3f *wi, Float *pdf_value,
                                     VisibilityTester *vis) const {
  // TODO impl in 14.2
  return Spectrum(0.0);
}
Spectrum DiffuseAreaLight::total_power() const {
  return m_L_emit * m_area * PI;
}
}  // namespace TRay