#pragma once
#include "core/TRay.h"
#include "core/Material.h"

namespace TRay {
/// @brief Matte material using Lambertian and Oren-Nayar model.
class MatteMaterial : public Material {
 public:
  MatteMaterial(const std::shared_ptr<Texture<Spectrum>> &diffuse,
                const std::shared_ptr<Texture<Float>> &sigma)
      : m_diffuse(diffuse), m_sigma(sigma) {};
  void fill_scattering_func(SurfaceInteraction *si, TransportMode mode,
                            bool allow_multi_lobes) const override;

 private:
  std::shared_ptr<Texture<Spectrum>> m_diffuse;
  std::shared_ptr<Texture<Float>> m_sigma;
};
}  // namespace TRay