#include "core/reflection/FresnelSpecular.h"
#include "core/Material.h"
#include "core/geometry/Point.h"

namespace TRay {

Spectrum SpecularReflection::f(const Vector3f &wo, const Vector3f &wi) const {
  return Spectrum(0.0);
}
Spectrum SpecularReflection::sample_f(const Vector3f &wo, Vector3f *wi,
                                      const Point2f &sample, Float *pdf_value,
                                      BxDFType *sampled_type) const {
  // Reflection direction for perfect speculat reflection. In BRDF space.
  *wi = Vector3f(-wo.x, -wo.y, wo.z);
  *pdf_value = 1.0;
  return m_fresnel->evaluate(cos_theta_of(*wi)) * m_scale /
         abs_cos_theta_of(*wi);
}
Float SpecularReflection::pdf(const Vector3f &wo, const Vector3f &wi) const {
  // Using delta distribution there is zero probability to find this direction.
  return 0;
}
std::string SpecularReflection::to_string() const {
  return "[ SpecularReflection scale: " + m_scale.to_string() +
         " fresnel: " + m_fresnel->to_string() + " SPECULAR | REFLECTION ]";
}

Spectrum SpecularTransmission::f(const Vector3f &wo, const Vector3f &wi) const {
  return Spectrum(0.0);
}
Spectrum SpecularTransmission::sample_f(const Vector3f &wo, Vector3f *wi,
                                        const Point2f &sample, Float *pdf_value,
                                        BxDFType *sampled_type) const {
  // Check the incident and transmit eta.
  bool entering = cos_theta_of(wo) > 0;
  Float eta_i = entering ? m_eta_A : m_eta_B;
  Float eta_t = entering ? m_eta_B : m_eta_A;

  // Compute ray direction for specular transmission.
  if (!refract(wo, align_with(Normal3f(0, 0, 1), wo), eta_i / eta_t, wi))
    return 0.0;
  *pdf_value = 1.0;
  Spectrum ft =
      m_scale * (Spectrum(1.0) - m_fresnel.evaluate(cos_theta_of(*wi)));
  // Account for non-symmetry with transmission to different medium.
  if (m_mode == TransportMode::Radiance)
    ft *= (eta_i * eta_i) / (eta_t * eta_t);
  return ft / abs_cos_theta_of(*wi);
}
Float SpecularTransmission::pdf(const Vector3f &wo, const Vector3f &wi) const {
  // Using delta distribution there is zero probability to find this direction.
  return 0;
}
std::string SpecularTransmission::to_string() const {
  return "[ SpecularTransmission scale: " + m_scale.to_string() +
         " fresnel: " + m_fresnel.to_string() +
         " etaA: " + format_one("%f", m_eta_A) +
         " etaB: " + format_one("%f", m_eta_B) + " mode : " +
         (m_mode == TransportMode::Radiance ? "RADIANCE" : "IMPORTANCE") + " SPECULAR | TRANSMISSION ]";
}

Spectrum FresnelSpecular::f(const Vector3f &wo, const Vector3f &wi) const {
  return Spectrum(0.0);
}
Spectrum FresnelSpecular::sample_f(const Vector3f &wo, Vector3f *wi,
                                   const Point2f &sample, Float *pdf_value,
                                   BxDFType *sampled_type) const {
  Float F = fresnel_dielectric(cos_theta_of(wo), m_eta_A, m_eta_B);
  if (F > sample[0]) {
    // Specular reflection.
    *wi = Vector3f(-wo.x, -wo.y, wo.z);
    if (sampled_type) *sampled_type = BxDFType(BSDF_SPECULAR | BSDF_REFLECTION);
    *pdf_value = F;
    return F * m_R_scale / abs_cos_theta_of(*wi);
  } else {
    // Check the incident and transmit eta.
    bool entering = cos_theta_of(wo) > 0;
    Float eta_i = entering ? m_eta_A : m_eta_B;
    Float eta_t = entering ? m_eta_B : m_eta_A;
    // Compute ray direction for specular transmission.
    if (!refract(wo, align_with(Normal3f(0, 0, 1), wo), eta_i / eta_t, wi))
      return 0;
    Spectrum ft = m_T_scale * (1 - F);
    // Non-symmetry with transmission to different medium.
    if (m_mode == TransportMode::Radiance)
      ft *= (eta_i * eta_i) / (eta_t * eta_t);
    if (sampled_type)
      *sampled_type = BxDFType(BSDF_SPECULAR | BSDF_TRANSMISSION);
    *pdf_value = 1 - F;
    return ft / abs_cos_theta_of(*wi);
  }
  return Spectrum(0.0);
}
std::string FresnelSpecular::to_string() const {
  return "[ FresnelSpecular R scale: " + m_R_scale.to_string() +
         " T scale: " + m_T_scale.to_string() +
         " fresnel: " + m_fresnel.to_string() +
         " etaA: " + format_one("%f", m_eta_A) +
         " etaB: " + format_one("%f", m_eta_B) + " mode : " +
         (m_mode == TransportMode::Radiance ? "RADIANCE" : "IMPORTANCE") + " ]";
}
}  // namespace TRay