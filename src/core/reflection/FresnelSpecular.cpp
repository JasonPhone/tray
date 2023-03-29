#include "core/reflection/FresnelSpecular.h"

namespace TRay {

Spectrum SpecularReflection::f(const Vector3f &wo, const Vector3f &wi) const {
  return Spectrum(0.0);
}
Spectrum SpecularReflection::sample_f(const Vector3f &wo, Vector3f *wi,
                                      const Point2f &sample, Float *pdf,
                                      BxDFType *sampled_type = nullptr) const {
  // Reflection direction for perfect speculat reflection. In BRDF space.
  *wi = Vector3f(-wo.x, -wo.y, wo.z);
  *pdf = 1.0;
  return m_fresnel->evaluate(cos_theta(*wi)) * m_scale / abs_cos_theta(*wi);
}

Spectrum SpecularTransmission::f(const Vector3f &wo, const Vector3f &wi) const {
  return Spectrum(0.0);
}
Spectrum SpecularTransmission::sample_f(
    const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float *pdf,
    BxDFType *sampled_type = nullptr) const {
  // Check the incident and transmit eta.
  bool entering = cos_theta(wo) > 0;
  Float eta_i = entering ? m_eta_A : m_eta_B;
  Float eta_t = entering ? m_eta_B : m_eta_A;

  // Compute ray direction for specular transmission
  if (!refract(wo, align_with(Normal3f(0, 0, 1), wo), eta_i / eta_t, wi))
    return 0.0;
  *pdf = 1.0;
  Spectrum ft = m_scale * (Spectrum(1.0) - m_fresnel.evaluate(cos_theta(*wi)));
  // Account for non-symmetry with transmission to different medium
  if (m_mode == TransportMode::Radiance)
    ft *= (eta_i * eta_i) / (eta_t * eta_t);
  return ft / abs_cos_theta(*wi);
}

Spectrum FresnelSpecular::f(const Vector3f &wo, const Vector3f &wi) const {
  return Spectrum(0.0);
}
Spectrum FresnelSpecular::sample_f(const Vector3f &wo, Vector3f *wi,
                                   const Point2f &sample, Float *pdf,
                                   BxDFType *sampled_type = nullptr) const {
  // TODO Impl is in pbrt 14.1.3.
  return Spectrum(0.0);
}
}  // namespace TRay