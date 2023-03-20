#pragma once
#include "core/TRay.h"
#include "core/geometry/Bound.h"
#include "core/geometry/Ray.h"
#include "core/geometry/Interaction.h"

namespace TRay {
class Primitive {
 public:
  /// @brief Get the world-space bound of the primivite's geometry.
  Bound3f world_bound() const { return do_world_bound(); }
  bool intersect(const Ray &ray, SurfaceInteraction *si) const {
    return do_intersect(ray, si);
  }
  bool intersect_test(const Ray &ray) const { return do_intersect_test(ray); }
  /// @brief Get the pointer to area light if this primitive is emissive.
  /// @return nullptr if this primitive is not a light source.
  const AreaLight *area_light() const { return do_area_light(); }
  /// @brief Get the pointer to material of this primitive.
  ///        Also used to test if ray hits the same object by comparing return
  ///        value.
  /// @return nullptr if this primitive is to cover participating media,
  ///         and the intersection SHOULD BE IGNORED.
  const Material *material() const { return do_material(); }
  /// @brief Compute the BxDF for the material at the intersection point.
  /// @todo Review the two params below to see if truly needed.
  /// @param mode Light transport mode, used in materials.
  /// @param allow_multi_lobes Detail of some BxDFs.
  void fill_scattering_func(SurfaceInteraction *si, TransportMode mode,
                            bool allow_multi_lobes) {
    do_fill_scattering_func(si, mode, allow_multi_lobes);
  }

 private:
  virtual Bound3f do_world_bound() const = 0;
  /**
   * @brief Primitive's intersect should (when a intersection is found):
   *        1. Update the t_max of a ray.
   *        2. Update the non-geometric info of surface interaction.
   */
  virtual bool do_intersect(const Ray &ray, SurfaceInteraction *si) const = 0;
  virtual bool do_intersect_test(const Ray &ray) const = 0;
  virtual AreaLight *do_area_light() const = 0;
  virtual Material *do_material() const = 0;
  virtual void do_fill_scattering_func(SurfaceInteraction *si,
                                       TransportMode mode,
                                       bool allow_multi_lobes) const = 0;
};

}  // namespace TRay