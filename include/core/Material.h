#pragma once
#include "core/TRay.h"

namespace TRay {
/// @brief The interaction is from camera or light source.
enum class TransportMode { Radiance, Importance };
/// @brief Material interface.
class Material {
 public:
  /// @param mode The interaction is from camera or light source.
  /// @param allow_multi_lobes true to allow mixed BxDFs like FresnelSpecular.
  virtual void fill_scattering_func(SurfaceInteraction *si, TransportMode mode,
                                    bool allow_multi_lobes) const = 0;
  virtual ~Material(){};

 private:
};
}  // namespace TRay