#pragma once
#include "core/TRay.h"
#include "core/geometry/Interaction.h"
#include "core/reflection/BxDF.h"
#include "core/spectrum/spectrum.h"

namespace TRay {
/// @brief Collection of BRDF and BTDF.
class BSDF {
 public:
  ~BSDF() {
    for (const BxDF *bxdf : m_BxDFs)
      if (bxdf) delete bxdf;
  }
  /// @param si Surface infomation.
  /// @param eta RELATIVE eta of the interface. 1.0 for obaque materials.
  BSDF(const SurfaceInteraction &si, Float eta = 1.0);
  void add_BxDF(BxDF &b);
  int count_BxDF(BxDFType flags) const;
  /// @brief Transforms a world vector into local shading space.
  Vector3f world_to_local(const Vector3f v_world) const;
  /// @brief Transforms a local shading vector into world space.
  Vector3f local_to_world(const Vector3f v_local) const;
  /// @brief The ordinary BSDF function.
  Spectrum f(const Vector3f &wo_world, const Vector3f wi_world,
             BxDFType flags = BSDF_ALL) const;
  Spectrum rho(const Vector3f &wo_world, int nsamples, const Point2f *samples,
               BxDFType flags = BSDF_ALL) const;
  Spectrum rho(int n_samples, const Point2f *samples1, const Point2f *samples2,
               BxDFType flags = BSDF_ALL) const;
  /// @brief BSDF function in MC style.
  Spectrum sample_f(const Vector3f &wo_world, Vector3f *wi_world,
                    const Point2f &u, Float *pdf_value,
                    BxDFType type = BSDF_ALL,
                    BxDFType *sampled_type = nullptr) const;
  Float pdf(const Vector3f &wo_world, const Vector3f &wi_world,
            BxDFType flags) const;
  /// @brief Get number of BxDFs covered by @param flags.
  int num_BxDFs(BxDFType flags) const {
    int cnt = 0;
    for (const auto &bxdf : m_BxDFs)
      if (bxdf->match_types(flags)) cnt++;
    return cnt;
  }

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