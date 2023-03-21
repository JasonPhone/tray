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
      : m_shape(_shape),
        m_material(_material),
        m_area_light(_area_light) {}

 private:
  Bound3f do_world_bound() const override;
  bool do_intersect(const Ray &ray, SurfaceInteraction *si) const override;
  bool do_intersect_test(const Ray &ray) const override;
  AreaLight *do_area_light() const override;
  Material *do_material() const override;
  void do_fill_scattering_func(SurfaceInteraction *si, TransportMode mode,
                               bool allow_multi_lobes) const override;
  std::shared_ptr<Shape> m_shape;
  std::shared_ptr<Material> m_material;
  std::shared_ptr<AreaLight> m_area_light;
};

}  // namespace TRay