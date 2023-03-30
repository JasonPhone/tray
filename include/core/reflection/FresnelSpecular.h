#pragma once
#include "core/TRay.h"
#include "core/reflection/Fresnel.h"
#include "core/reflection/BxDF.h"

namespace TRay {
/**
 * @brief Specular reflection BRDF based on Fresnel formula.
 *        A delta distribution is used to represent the distribution function.
 */
class SpecularReflection : public BxDF {
 public:
  SpecularReflection(const Spectrum &scale, Fresnel *fresnel)
      : BxDF(BxDFType(BSDF_SPECULAR | BSDF_REFLECTION)),
        m_scale(scale),
        m_fresnel(fresnel) {}
  // No scattering is returned by f().
  Spectrum f(const Vector3f &wo, const Vector3f &wi) const override;
  Spectrum sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample,
                    Float *pdf, BxDFType *sampled_type) const override;
  std::string to_string() const override;

 private:
  // Reflection scale factor.
  const Spectrum m_scale;
  const Fresnel *m_fresnel;
};
/**
 * @brief Specular transmission BTDF based on Fresnel formula.
 *        Almost the same as SpecularReflection.
 */
class SpecularTransmission : public BxDF {
 public:
  /// @brief Create a transmission BRDF.
  /// @param eta_A Index of fraction of the material at same side of normal.
  /// @param eta_B Index of fraction of the material at opposite side of normal.
  SpecularTransmission(const Spectrum &scale, Float eta_A, Float eta_B,
                       TransportMode mode)
      : BxDF(BxDFType(BSDF_SPECULAR | BSDF_TRANSMISSION)),
        m_scale(scale),
        m_eta_A(eta_A),
        m_eta_B(eta_B),
        m_fresnel(eta_A, eta_B),
        m_mode(mode) {}
  // No scattering is returned by f().
  Spectrum f(const Vector3f &wo, const Vector3f &wi) const override;
  Spectrum sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample,
                    Float *pdf, BxDFType *sampled_type) const override;
  std::string to_string() const override;

 private:
  // Transmission scale factor.
  const Spectrum m_scale;
  // Transmission happens at the interface,
  // we cannot figure out the in-out relation now.
  const Float m_eta_A, m_eta_B;
  const FresnelDielectric m_fresnel;
  const TransportMode m_mode;
};

/**
 * @brief Represents both specular reflection and speculat transmission.
 *
 */
class FresnelSpecular : public BxDF {
 public:
  FresnelSpecular(const Spectrum &R, const Spectrum &T, Float eta_A,
                  Float eta_B, TransportMode mode)
      : BxDF(BxDFType(BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION)),
        m_R_scale(R),
        m_T_scale(T),
        m_eta_A(eta_A),
        m_eta_B(eta_B),
        m_fresnel(eta_A, eta_B),
        m_mode(mode) {}
  Spectrum f(const Vector3f &wo, const Vector3f &wi) const override;
  Spectrum sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample,
                    Float *pdf, BxDFType *sampled_type) const override;
  std::string to_string() const override;

 private:
  const Spectrum m_R_scale, m_T_scale;
  const Float m_eta_A, m_eta_B;
  const FresnelDielectric m_fresnel;
  const TransportMode m_mode;
};
}  // namespace TRay