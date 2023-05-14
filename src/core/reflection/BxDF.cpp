#include "core/reflection/BxDF.h"
#include "core/math/sampling.h"
namespace TRay {
/// @details The default impl samples the hemisphere with cosine distribution.
Spectrum BxDF::sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample,
                        Float *pdf_value, BxDFType *) const {
  // Sample hemisphere around (0, 0, 1).
  *wi = hemisphere_cosine_sample(sample);
  if (wo.z < 0) wi->z *= -1;
  *pdf_value = pdf(wo, *wi);
  return f(wo, *wi);
}
Spectrum BxDF::rho(const Vector3f &wo, int n_samples,
                   const Point2f *samples) const {
  Spectrum r(0.0);
  Vector3f wi;
  Float pdf_wi = 0;
  for (int i = 0; i < n_samples; i++) {
    Spectrum f = sample_f(wo, &wi, samples[i], &pdf_wi);
    if (pdf_wi > 0) r += f * abs_cos_theta_of(wi) / pdf_wi;
  }
  return r / n_samples;
}
Spectrum BxDF::rho(int n_samples, const Point2f *samples1,
                   const Point2f *samples2) const {
  Spectrum r(0.0);
  Vector3f wi, wo;
  Float pdf_wi = 0;
  for (int i = 0; i < n_samples; i++) {
    wo = hemisphere_uniform_sample(samples1[i]);
    Float pdf_wo = hemisphere_uniform_pdf();
    Spectrum f = sample_f(wo, &wi, samples2[i], &pdf_wi);
    if (pdf_wi > 0)
      r += f * abs_cos_theta_of(wi) * abs_cos_theta_of(wo) / (pdf_wi * pdf_wo);
  }
  return r / (PI * n_samples);
}
Float BxDF::pdf(const Vector3f &wo, const Vector3f &wi) const {
  // Check the same hemisphere.
  return (wo.z * wi.z > 0) ? (abs_cos_theta_of(wi) * PI_INV) : 0;
}

Spectrum ScaledBxDF::rho(const Vector3f &w, int nSamples,
                         const Point2f *samples) const {
  return m_scale * m_bxdf->rho(w, nSamples, samples);
}
Spectrum ScaledBxDF::rho(int n_samples, const Point2f *samples1,
                         const Point2f *samples2) const {
  return m_scale * m_bxdf->rho(n_samples, samples1, samples2);
}
Spectrum ScaledBxDF::f(const Vector3f &wo, const Vector3f &wi) const {
  return m_scale * m_bxdf->f(wo, wi);
}
Spectrum ScaledBxDF::sample_f(const Vector3f &wo, Vector3f *wi,
                              const Point2f &sample, Float *pdf_value,
                              BxDFType *sampled_type) const {
  Spectrum f = m_bxdf->sample_f(wo, wi, sample, pdf_value, sampled_type);
  return m_scale * f;
}
Float ScaledBxDF::pdf(const Vector3f &wo, const Vector3f &wi) const {
  return m_bxdf->pdf(wo, wi);
}
std::string ScaledBxDF::to_string() const {
  return "[ ScaledBxDF bxdf: " + m_bxdf->to_string() +
         " scale: " + m_scale.to_string() + " ]";
}
}  // namespace TRay