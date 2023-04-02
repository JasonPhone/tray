#include "core/reflection/OrenNayar.h"
/**
 * @brief OrenNayar microfacet BRDF, which consider the microsurfaces as
 *        Lambertian.
 * Approximation formula:
 * f_r(w_i, w_o) = R/pi (A + B max(0, cos(phi_i - phi_o))sin(alpha)tan(beta))
 *   A = 1 - sigma^2 / 2(sigma^2 + 0.33)
 *   B = 0.45 sigma^2 / (sigma^2 + 0.09)
 *   alpha = max(theta_i, theta_o)
 *   beta = min(theta_i, theta_o)
 *   sigma is in radians
 */
namespace TRay {
OrenNayar::OrenNayar(const Spectrum &scale, Float sigma)
    : BxDF(BxDFType(BSDF_DIFFUSE | BSDF_REFLECTION)), m_scale(scale) {
  sigma = deg_to_rad(sigma);
  Float sigma2 = sigma * sigma;
  m_A = 1.0 - 0.5 * sigma2 / (sigma2 + 0.33);
  m_B = 0.45 * sigma2 / (sigma2 + 0.09);
}
Spectrum OrenNayar::f(const Vector3f &wo, const Vector3f &wi) const {
  Float sin_thetai = sin_theta_of(wi);
  Float sin_thetao = sin_theta_of(wo);
  // Cosine term.
  Float max_cos = 0;
  if (sin_thetai > 1e-4 && sin_thetao > 1e-4) {
    Float sin_phii = sin_phi_of(wi), cos_phii = cos_phi_of(wi);
    Float sin_phio = sin_phi_of(wo), cos_phio = cos_phi_of(wo);
    Float cos_val = cos_phii * cos_phio + sin_phii * sin_phio;
    max_cos = std::max((Float)0, cos_val);
  }
  // Sine and tangent terms.
  Float sin_alpha, tan_beta;
  if (abs_cos_theta_of(wi) > abs_cos_theta_of(wo)) {
    sin_alpha = sin_thetao;
    tan_beta = sin_thetai / abs_cos_theta_of(wi);
  } else {
    sin_alpha = sin_thetai;
    tan_beta = sin_thetao / abs_cos_theta_of(wo);
  }
  return m_scale * PI_INV * (m_A + m_B * max_cos * sin_alpha * tan_beta);
}

std::string OrenNayar::to_string() const {
  return "[ OrenNayar scale: " + m_scale.to_string() +
         string_format(" A: %f B: %f ]", m_A, m_B);
}
}  // namespace TRay