#pragma once
#include "core/TRay.h"
#include "core/geometry/Vector.h"
#include "core/geometry/Normal.h"
#include "core/spectrum/spectrum.h"

namespace TRay {
// Theta is with positive z, phi is with positive x.
// The left-handed frame is by x right, y backward and z upward.
inline Float cos_theta_of(const Vector3f &w) { return w.z; }
inline Float cos2_theta_of(const Vector3f &w) { return w.z * w.z; }
inline Float abs_cos_theta_of(const Vector3f &w) { return std::abs(w.z); }
inline Float sin2_theta_of(const Vector3f &w) {
  return std::max((Float)0, (Float)1 - cos2_theta_of(w));
}
inline Float sin_theta_of(const Vector3f &w) {
  return std::sqrt(sin2_theta_of(w));
}
inline Float tan_theta_of(const Vector3f &w) {
  return sin_theta_of(w) / cos_theta_of(w);
}
inline Float tan2_theta_of(const Vector3f &w) {
  return sin2_theta_of(w) / cos2_theta_of(w);
}
inline Float cos_phi_of(const Vector3f &w) {
  Float sin_t = sin_theta_of(w);
  return (sin_t == 0) ? 1 : clamp(w.x / sin_t, -1, 1);
}
inline Float sin_phi_of(const Vector3f &w) {
  Float sin_t = sin_theta_of(w);
  return (sin_t == 0) ? 0 : clamp(w.y / sin_t, -1, 1);
}
inline Float cos2_phi_of(const Vector3f &w) {
  return cos_phi_of(w) * cos_phi_of(w);
}
inline Float sin2_phi_of(const Vector3f &w) {
  return sin_phi_of(w) * sin_phi_of(w);
}
/// @brief Cosine value of difference of phi of two direction.
inline Float cos_delta_phi_of(const Vector3f &wa, const Vector3f &wb) {
  Float waxy = wa.x * wa.x + wa.y * wa.y;
  Float wbxy = wb.x * wb.x + wb.y * wb.y;
  if (waxy == 0 || wbxy == 0) return 1;
  return clamp((wa.x * wb.x + wa.y * wb.y) / std::sqrt(waxy * wbxy), -1, 1);
}
/// @brief Get the reflect direction. Actually the incident direction.
/// @param wo Reflected dirction.
/// @param n Surface normal.
/// @return Incident direction.
inline Vector3f reflect(const Vector3f &wo, const Vector3f &n) {
  return -wo + 2 * dot(wo, n) * n;
}
/// @brief Get the refracted direction.
/// @param wi Incident direction.
/// @param n Surface normal.
/// @param eta Proportion of two etas. eta_i / eta_t.
/// @param wt Place to store the refracted direction.
/// @return false if total internal reflection happens.
inline bool refract(const Vector3f &wi, const Normal3f &n, Float eta,
                    Vector3f *wt) {
  // Snell's law.
  Float cos_theta_i = dot(n, wi);
  Float sin2_theta_i = std::max(Float(0), Float(1 - cos_theta_i * cos_theta_i));
  Float sin2_theta_t = eta * eta * sin2_theta_i;

  // Total internal reflection for transmission.
  if (sin2_theta_t >= 1) return false;
  Float cos_theta_t = std::sqrt(1 - sin2_theta_t);
  *wt = eta * -wi + (eta * cos_theta_i - cos_theta_t) * Vector3f(n);
  return true;
}

enum BxDFType {
  BSDF_REFLECTION = 1 << 0,
  BSDF_TRANSMISSION = 1 << 1,
  BSDF_DIFFUSE = 1 << 2,
  BSDF_GLOSSY = 1 << 3,
  BSDF_SPECULAR = 1 << 4,
  BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION |
             BSDF_TRANSMISSION,
};
/// @brief Interface for BRDF and BTDF.
class BxDF {
 public:
  virtual ~BxDF() {}
  BxDF(BxDFType type) : m_type(type) {}
  /// @brief See if this BxDF is COVERED given types.
  bool match_types(BxDFType t) const { return (m_type & t) == m_type; }
  /// @brief Return Spectrum value of the distribution function based on wi and
  ///        wo. Assuming the wavelengths are decoupled.
  virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const = 0;
  /// @brief Samples a wi given wo and returns the value for them.
  /// @note Overriding sample_f() MUST override pdf() for consistent result.
  virtual Spectrum sample_f(const Vector3f &wo, Vector3f *wi,
                            const Point2f &sample, Float *pdf_value,
                            BxDFType *sampled_type = nullptr) const;
  /// @note Overriding sample_f() MUST override pdf() for consistent result.
  virtual Float pdf(const Vector3f &wo, const Vector3f &wi) const;
  /// @brief hemispherical-directional reflectance (ratio of flux), rho_hd,
  ///        albedo. The total reflection in
  ///        given direction when illumination is constant over the hemisphere.
  ///        Or the total reflectance over the hemisphere when light comes from
  ///        one direction. The latter one is true only if the BxDF is
  ///        reciprocal.
  /// @param wo Direction.
  /// @param n_samples Samples are used by MCM to compute an approx value.
  /// @param samples
  virtual Spectrum rho(const Vector3f &wo, int n_samples,
                       const Point2f *samples) const;
  /// @brief hemisherical-hemispherical reflectance (ratio of flux), rho_hh. The
  ///        total reflection in
  ///        all directions when illumination is constant over the hemisphere.
  /// @param n_samples Samples are used by MCM to compute an approx value.
  /// @param samples1
  /// @param samples2
  virtual Spectrum rho(int n_samples, const Point2f *samples1,
                       const Point2f *samples2) const;

  virtual std::string to_string() const = 0;

  const BxDFType m_type;
};

inline std::ostream &operator<<(std::ostream &os, const BxDF &bxdf) {
  os << bxdf.to_string();
  return os;
}

/// @brief BxDF able to be scaled by a Spectrum class, for the weighted
///        combination of different materials.
class ScaledBxDF : public BxDF {
 public:
  ScaledBxDF(BxDF *bxdf, const Spectrum &scale)
      : BxDF(BxDFType(bxdf->m_type)), m_bxdf(bxdf), m_scale(scale) {}
  Spectrum rho(const Vector3f &w, int nSamples,
               const Point2f *samples) const override;
  Spectrum rho(int n_samples, const Point2f *samples1,
               const Point2f *samples2) const override;
  Spectrum f(const Vector3f &wo, const Vector3f &wi) const override;
  Spectrum sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample,
                    Float *pdf_value, BxDFType *sampled_type) const override;
  Float pdf(const Vector3f &wo, const Vector3f &wi) const override;
  std::string to_string() const override;

 private:
  BxDF *m_bxdf;
  Spectrum m_scale;
};
}  // namespace TRay