#pragma once
#include "core/TRay.h"
#include "core/geometry/Interaction.h"
#include "core/geometry/Bound.h"

namespace TRay {
/// @brief Shape interface.
/**
 * All shapes are in their object space. The transform between obj space and
 * world space is stored in Shape class.
 */
class Shape {
 public:
  Shape(const Transform &obj_world, const Transform &world_obj, bool flip_n)
      : obj_to_world(obj_world),
        world_to_obj(world_obj),
        flip_normal(flip_n),
        swap_handness(obj_world.will_swap_hand()) {}
  Bound3f object_bound() const { return do_object_bound(); }
  Bound3f world_bound() const { return obj_to_world(object_bound()); }
  /**
   * @brief Get the first intersect in (0, t_max).
   *
   * @param ray Incoming ray.
   * @param time Return first hit time.
   * @param inter Return hit point interaction.
   * @param test_alpha_texture If test transparent texture.
   */
  bool intersect(const Ray &ray, Float *time, SurfaceInteraction *inter,
                 bool test_alpha_texture = true) const {
    return do_intersect(ray, time, inter, test_alpha_texture);
  }
  /// @brief Just test without getting any detailed info.
  bool intersect_test(const Ray &ray, bool test_alpha_texture = true) const {
    return do_intersect_test(ray, test_alpha_texture);
  }
  Float area() const { return do_area(); }
  const Transform &obj_to_world, &world_to_obj;
  const bool flip_normal;
  const bool swap_handness;

 private:
  /// @brief Non-virtual interface.
  virtual Bound3f do_object_bound() const = 0;
  /**
   * Some points in doing intersection:
   *  1. Intersection after ray.t_max is ignored.
   *  2. Use *time to return time of the FIRST hit.
   *  3. Interaction is used widely to separate geometry code and shading code.
   *  4. Ray is in world space, but intersection test is easier in object space,
   *     and the return interaction info should be in world space.
   */
  virtual bool do_intersect(const Ray &r, Float *time,
                            SurfaceInteraction *inter,
                            bool test_alpha_texture = true) const = 0;
  virtual bool do_intersect_test(const Ray &ray,
                                 bool test_alpha_texture = true) const {
    // Should use better method.
    Float t_hit = ray.t_max;
    SurfaceInteraction inter;
    return do_intersect(ray, &t_hit, &inter, test_alpha_texture);
  }
  virtual Float do_area() const = 0;
};

}  // namespace TRay