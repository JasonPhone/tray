#pragma once
#include "core/TRay.h"
#include "core/Light.h"

namespace TRay {
class DistantLight : public Light {
 public:
  /// @brief A light source infinite far away that emits parallel light.
  ///        Has no geometric shape to attach.
  /// @param L Color of the light.
  /// @param w_light Light coming from this direction.
  DistantLight(const Transform &light_to_world, const Spectrum &L,
               const Vector3f w_light)
      : Light(LIGHT_DELTA_DIRECTION, light_to_world),
        m_L(L),
        m_w_light(normalize(light_to_world(w_light))) {}
  Spectrum sample_Li(const Interaction &ref, const Point2f &u, Vector3f *wi,
                     Float *pdf_value, VisibilityTester *vis) const override;
  Float pdf_Li(const Interaction &ref, const Vector3f &wi) const override;
  Spectrum total_power() const override;
  void preprocess(const Scene &scene) override;

 private:
  // Light L.
  const Spectrum m_L;
  // Light direction.
  const Vector3f m_w_light;
  // Scene properties, a bounding sphere.
  Point3f m_world_center;
  Float m_world_radius;
};
}  // namespace TRay