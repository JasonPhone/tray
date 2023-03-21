#pragma once
#include "core/primitives/Primitive.h"
#include "core/geometry/AnimateTransform.h"
// AnimateTransform is applied here, not inside with Shape.

namespace TRay {
class TransformedPrimitive : public Primitive {
 public:
  /// @brief Construct a GeometricPrimitive.
  /// @param sh Shape.
  /// @param mt Material.
  /// @param al AreaLight, nullptr if this primitive is not emissive.
  TransformedPrimitive(std::shared_ptr<Primitive> &_primitive,
                       const AnimateTransform &_prim_to_world)
      : m_primitive(_primitive), m_prim_to_world(_prim_to_world) {}

 private:
  Bound3f do_world_bound() const override;
  bool do_intersect(const Ray &ray, SurfaceInteraction *si) const override;
  bool do_intersect_test(const Ray &ray) const override;
  AreaLight *do_area_light() const override;
  Material *do_material() const override;
  void do_fill_scattering_func(SurfaceInteraction *si, TransportMode mode,
                               bool allow_multi_lobes) const override;
  std::shared_ptr<Primitive> m_primitive;
  const AnimateTransform m_prim_to_world;
};
}  // namespace TRay