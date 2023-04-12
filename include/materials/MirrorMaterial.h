#pragma once
#include "core/TRay.h"
#include "core/Material.h"
#include "core/reflection/FresnelSpecular.h"

namespace TRay {
/// @brief Perfect mirror, reflect anything incident.
class MirrorMaterial : public Material {
 public:
  MirrorMaterial() {}
  void fill_scattering_func(SurfaceInteraction *si, TransportMode mode,
                            bool allow_multi_lobes) const override;

 private:
};
}  // namespace TRay