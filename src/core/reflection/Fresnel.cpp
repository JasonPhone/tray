#include "core/reflection/Fresnel.h"
#include "core/spectrum/spectrum.h"

namespace TRay {

Float fresnel_dielectric(Float cos_theta_i, Float eta_i, Float eta_t) {
  cos_theta_i = clamp(cos_theta_i, -1, 1);
  // Figure the in and out.
  bool entering = cos_theta_i > 0.f;
  if (!entering) {
    std::swap(eta_i, eta_t);
    cos_theta_i = std::abs(cos_theta_i);
  }
  // Snell's law.
  Float sin_theta_i =
      std::sqrt(std::max((Float)0, 1 - cos_theta_i * cos_theta_i));
  Float sin_theta_t = eta_i / eta_t * sin_theta_i;
  // Total internal reflection.
  if (sin_theta_t >= 1) return 1;
  // Fresnel formula without imaginary part.
  Float cos_theta_t =
      std::sqrt(std::max((Float)0, 1 - sin_theta_t * sin_theta_t));
  Float ref_para = ((eta_t * cos_theta_i) - (eta_i * cos_theta_t)) /
                   ((eta_t * cos_theta_i) + (eta_i * cos_theta_t));
  Float ref_perp = ((eta_i * cos_theta_i) - (eta_t * cos_theta_t)) /
                   ((eta_i * cos_theta_i) + (eta_t * cos_theta_t));
  return (ref_para * ref_para + ref_perp * ref_perp) / 2;
}
// https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
Spectrum fresnel_conductor(Float cos_theta_i, const Spectrum &eta_i,
                           const Spectrum &eta_t, const Spectrum &k) {
  cos_theta_i = clamp(cos_theta_i, -1, 1);
  // No need to check direction, light may not goes out from conductor.
  // So as the complex division reduced into real-valued.
  Spectrum eta = eta_t / eta_i;
  Spectrum etak = k / eta_i;

  Float cos_theta_i2 = cos_theta_i * cos_theta_i;
  Float sin2_theta_i = 1. - cos_theta_i2;
  Spectrum eta2 = eta * eta;
  Spectrum etak2 = etak * etak;

  Spectrum t0 = eta2 - etak2 - sin2_theta_i;
  Spectrum a2plusb2 = sqrt(t0 * t0 + 4 * eta2 * etak2);
  Spectrum t1 = a2plusb2 + cos_theta_i2;
  Spectrum a = sqrt(0.5f * (a2plusb2 + t0));
  Spectrum t2 = (Float)2 * cos_theta_i * a;
  Spectrum ref_perp = (t1 - t2) / (t1 + t2);

  Spectrum t3 = cos_theta_i2 * a2plusb2 + sin2_theta_i * sin2_theta_i;
  Spectrum t4 = t2 * sin2_theta_i;
  Spectrum ref_para = ref_perp * (t3 - t4) / (t3 + t4);

  return 0.5 * (ref_para + ref_perp);
}

Spectrum FresnelConductor::evaluate(Float cos_i) const {
  return fresnel_conductor(cos_i, m_eta_i, m_eta_t, m_k);
}
std::string FresnelConductor::to_string() const {
  return "[ FresnelConductor etaI: " + m_eta_i.to_string() +
         "etaT: " + m_eta_t.to_string() + "k: " + m_k.to_string() + " ]";
}

Spectrum FresnelDielectric::evaluate(Float cos_i) const {
  return fresnel_dielectric(cos_i, m_eta_i, m_eta_t);
}
std::string FresnelDielectric::to_string() const {
  return "[ FresnelConductor etaI: " + format_one("%f", m_eta_i) +
         "etaT: " + format_one("%f", m_eta_t) + " ]";
}
}  // namespace TRay