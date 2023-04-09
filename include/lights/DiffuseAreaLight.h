#pragma once
#include "core/Light.h"
#include "core/geometry/Shape.h"

namespace TRay {
/// @brief Uniform spatial and directional radiance. Light goes along normals.
class DiffuseAreaLight : public AreaLight {
 public:
  DiffuseAreaLight(const Transform &LightToWorld, const Spectrum &Lemit,
                   int nSamples, const std::shared_ptr<Shape> &shape)
      : AreaLight(LightToWorld, nSamples),
        m_L_emit(Lemit),
        m_shape(shape),
        m_area(shape->area()) {}
  Spectrum L(const Interaction &inter, const Vector3f &w) const override;
  Spectrum sample_Li(const Interaction &ref, const Point2f &u, Vector3f *wi,
                     Float *pdf_value, VisibilityTester *vis) const override;
  Float pdf_Li(const Interaction &ref, const Vector3f &wi) const override;
  Spectrum total_power() const override;

 protected:
  const Spectrum m_L_emit;
  std::shared_ptr<Shape> m_shape;
  const Float m_area;
};
}  // namespace TRay