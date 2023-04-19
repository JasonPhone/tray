#pragma once
#include "core/TRay.h"
#include "core/Material.h"
#include "core/reflection/FresnelSpecular.h"

namespace TRay {
/// @brief Perfect mirror, reflect anything incident.
class MirrorMaterial : public Material {
 public:
  MirrorMaterial(const std::shared_ptr<Texture<Spectrum>> &color)
      : m_color{color} {}
  void fill_scattering_func(SurfaceInteraction *si, TransportMode mode,
                            bool allow_multi_lobes) const override;

 private:
  std::shared_ptr<Texture<Spectrum>> m_color;
};
}  // namespace TRay