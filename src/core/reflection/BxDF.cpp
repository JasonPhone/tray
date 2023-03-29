#include "core/reflection/BxDF.h"
namespace TRay {

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
                              const Point2f &sample, Float *pdf,
                              BxDFType *sampled_type) const {
  Spectrum f = m_bxdf->sample_f(wo, wi, sample, pdf, sampled_type);
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