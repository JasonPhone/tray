#include "lights/DiffuseAreaLight.h"

namespace TRay {

Spectrum DiffuseAreaLight::L(const Interaction &inter,
                             const Vector3f &w) const {
  return dot(inter.n, w) > 0.0 ? m_L_emit : Spectrum(0.0);
}
Spectrum DiffuseAreaLight::sample_Li(const Interaction &ref, const Point2f &u,
                                     Vector3f *wi, Float *pdf_value,
                                     VisibilityTester *vis) const {
  Interaction inter = m_shape->sample_surface(ref, u, pdf_value);
  *wi = normalize(inter.p - ref.p);
  *vis = VisibilityTester(ref, inter);
  return L(inter, -(*wi));
}
Float DiffuseAreaLight::pdf_Li(const Interaction &ref,
                               const Vector3f &wi) const {
  return m_shape->pdf(ref, wi);
}
Spectrum DiffuseAreaLight::total_power() const {
  return m_L_emit * m_area * PI;
}
}  // namespace TRay