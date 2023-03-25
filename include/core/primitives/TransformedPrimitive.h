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
  Bound3f world_bound() const override;
  bool intersect(const Ray &ray, SurfaceInteraction *si) const override;
  bool intersect_test(const Ray &ray) const override;
  AreaLight *area_light() const override;
  Material *material() const override;
  void fill_scattering_func(SurfaceInteraction *si, TransportMode mode,
                               bool allow_multi_lobes) const override;

 private:
  std::shared_ptr<Primitive> m_primitive;
  const AnimateTransform m_prim_to_world;
};
}  // namespace TRay