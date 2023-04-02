#pragma once
#include "core/TRay.h"
#include "core/reflection/BxDF.h"
/// @brief One from microfacet methods.
namespace TRay {
class OrenNayar : public BxDF {
 public:
  /// @param scale For basic class BxDF.
  /// @param sigma Standard deviation of microfacet orientation angle(degree).
  OrenNayar(const Spectrum &scale, Float sigma);
  Spectrum f(const Vector3f &wo, const Vector3f &wi) const override;
  std::string to_string() const override;

 private:
  Float m_A, m_B;
  const Spectrum m_scale;
};
}  // namespace TRay