#pragma once
#include "core/TRay.h"
#include "core/Material.h"
namespace TRay {
// TODO This class is incomplete. More microfacet models needed.
class PlasticMaterial : public Material {
 public:
  /// @param diffuse Diffuse texture.
  /// @param specular Specular texture.
  /// @param roughness Roughness value, used to control the alpha of microfacet
  /// distribution.
  /// @param remap_roughness true if the roughness value should be remapped.
  PlasticMaterial(const std::shared_ptr<Texture<Spectrum>> &diffuse,
                  const std::shared_ptr<Texture<Spectrum>> &specular,
                  const std::shared_ptr<Texture<Float>> &roughness,
                  bool remap_roughness)
      : m_diffuse(diffuse),
        m_specular(specular),
        m_roughness(roughness),
        m_remap_roughness(remap_roughness) {}
  void fill_scattering_func(SurfaceInteraction *si, TransportMode mode,
                            bool allow_multi_lobes) const override;

 private:
  std::shared_ptr<Texture<Spectrum>> m_diffuse, m_specular;
  std::shared_ptr<Texture<Float>> m_roughness;
  const bool m_remap_roughness;
};
}  // namespace TRay