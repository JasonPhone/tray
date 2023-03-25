#pragma once
#include "core/TRay.h"
#include "core/primitives/Primitive.h"

namespace TRay {
class GeometricPrimitive : public Primitive {
 public:
  /// @brief Construct a GeometricPrimitive.
  /// @param sh Shape.
  /// @param mt Material.
  /// @param al AreaLight, nullptr if this primitive is not emissive.
  GeometricPrimitive(const std::shared_ptr<Shape> &_shape,
                     const std::shared_ptr<Material> &_material,
                     const std::shared_ptr<AreaLight> &_area_light)
      : m_shape(_shape), m_material(_material), m_area_light(_area_light) {}
  Bound3f world_bound() const override;
  bool intersect(const Ray &ray, SurfaceInteraction *si) const override;
  bool intersect_test(const Ray &ray) const override;
  AreaLight *area_light() const override;
  Material *material() const override;
  void fill_scattering_func(SurfaceInteraction *si, TransportMode mode,
                            bool allow_multi_lobes) const override;

 private:
  std::shared_ptr<Shape> m_shape;
  std::shared_ptr<Material> m_material;
  std::shared_ptr<AreaLight> m_area_light;
};

}  // namespace TRay