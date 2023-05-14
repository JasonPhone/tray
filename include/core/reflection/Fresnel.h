#pragma once
#include "core/TRay.h"
#include "core/spectrum/spectrum.h"

namespace TRay {
/// @brief Get the Fresnel reflectance of dielectric.
/// @param cos_theta_i Cosine of incident angle.
/// @param eta_i Index of refraction of incident material.
/// @param eta_t Index of refraction of transmitted material.
/// @return We don't consider polarization, the value is avg of squares.
Float fresnel_dielectric(Float cos_theta_i, Float eta_i, Float eta_t);
/// @brief Get the Fresnel reflectance of conductor. Called ONLY when the index
/// has imaginary part. This is wave-dependent.
/// @param cos_theta_i Cosine of incident angle.
/// @param eta_i Indices of refraction of incident material.
/// @param eta_t Indices of refraction of transmitted material.
/// @param k Absorption coefficients.
Spectrum fresnel_conductor(Float cos_theta_i, const Spectrum &eta_i,
                           const Spectrum &eta_t, const Spectrum &k);

/// @brief Interface to compute the Fresnel coefficients.
class Fresnel {
 public:
  virtual ~Fresnel() {}
  /// @brief Get the coefficients.
  /// @param cos_i Cosine of incident angle.
  virtual Spectrum evaluate(Float cos_i) const = 0;
  virtual std::string to_string() const = 0;
};
/// @brief Interface to compute the Fresnel coefficients for conductor.
class FresnelConductor : public Fresnel {
 public:
  /// @param etai Indices of refraction of incident material.
  /// @param etat Indices of refraction of transmitted material.
  /// @param k Absorption coefficients.
  FresnelConductor(const Spectrum &etai, const Spectrum &etat,
                   const Spectrum &k)
      : m_eta_i(etai), m_eta_t(etat), m_k(k) {}
  Spectrum evaluate(Float cos_i) const override;
  std::string to_string() const override;

 private:
  Spectrum m_eta_i, m_eta_t, m_k;
};
/// @brief Interface to compute the Fresnel coefficients for dielectric.
class FresnelDielectric : public Fresnel {
 public:
  /// @param etai Index of refraction of incident material.
  /// @param etat Index of refraction of transmitted material.
  FresnelDielectric(const Float &etai, const Float &etat)
      : m_eta_i(etai), m_eta_t(etat) {}
  Spectrum evaluate(Float cos_i) const override;
  std::string to_string() const override;

 private:
  Float m_eta_i, m_eta_t;
};
/// @brief Reflect 100% light to all direction. Util Class.
class FresnelConst : public Fresnel {
 public:
  Spectrum evaluate(Float) const override { return Spectrum(1.0); }
  std::string to_string() const override { return "[ FresnelConst 1.0 ]"; }
};
}  // namespace TRay