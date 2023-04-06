#pragma once
#include "core/TRay.h"
#include "core/geometry/Interaction.h"
#include "core/reflection/BxDF.h"
#include "core/spectrum/spectrum.h"

namespace TRay {
/// @brief Collection of BRDF and BTDF.
class BSDF {
 public:
  /// @param si Surface infomation.
  /// @param eta RELATIVE eta of the interface. 1.0 for obaque materials.
  BSDF(const SurfaceInteraction &si, Float eta = 1.0);
  void add_BxDF(BxDF &b);
  int count_BxDF(BxDFType flags) const;
  /// @brief Transforms a world vector into local shading space.
  Vector3f world_to_local(const Vector3f v_world) const;
  /// @brief Transforms a local shading vector into world space.
  Vector3f local_to_world(const Vector3f v_local) const;
  Spectrum f(const Vector3f &wo_world, const Vector3f wi_world,
             BxDFType flags = BSDF_ALL) const;
  Spectrum rho(const Vector3f &wo_world, int nsamples, const Point2f *samples,
               BxDFType flags = BSDF_ALL) const;
  Spectrum rho(int n_samples, const Point2f *samples1, const Point2f *samples2,
               BxDFType flags = BSDF_ALL) const;
  Spectrum sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
                    Float *pdf, BxDFType type = BSDF_ALL,
                    BxDFType *sampled_type = nullptr) const;

  const Float m_eta;

 private:
  // Avoid the delete attempt if a custom memory pool is used.
  // ~BSDF();
  // Normal in shading space and geometry space.
  const Normal3f m_normal_s, m_normal_g;
  const Vector3f m_s_tan, m_s_bitan;
  std::vector<const BxDF *> m_BxDFs;
};
}  // namespace TRay