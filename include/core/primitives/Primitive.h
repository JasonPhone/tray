#pragma once
#include "core/geometry/Bound.h"
#include "core/geometry/Ray.h"
#include "core/geometry/Interaction.h"

namespace TRay {
class Primitive {
 public:
  /// @brief Get the world-space bound of the primivite's geometry.
  virtual Bound3f world_bound() const = 0;
  /**
   * @brief Primitive's intersect should (when a intersection is found):
   *        1. Update the t_max of a ray.
   *        2. Update the non-geometric info of surface interaction.
   */
  virtual bool intersect(const Ray &ray, SurfaceInteraction *si) const = 0;
  virtual bool intersect_test(const Ray &ray) const = 0;
  /// @brief Get the pointer to area light if this primitive is emissive.
  /// @return nullptr if this primitive is not a light source.
  virtual const AreaLight *area_light() const = 0;
  /// @brief Get the pointer to material of this primitive.
  ///        Also used to test if ray hits the same object by comparing return
  ///        value.
  /// @return nullptr if this primitive is to cover participating media,
  ///         and the intersection SHOULD BE IGNORED.
  virtual const Material *material() const = 0;
  /// @brief Compute the BxDF for the material at the intersection point.
  /// @todo Review the two params below to see if truly needed.
  /// @param mode Light transport mode, used in materials.
  /// @param allow_multi_lobes true to allow mixed BxDFs like FresnelSpecular.
  virtual void fill_scattering_func(SurfaceInteraction *si, TransportMode mode,
                                    bool allow_multi_lobes) const = 0;
};

}  // namespace TRay