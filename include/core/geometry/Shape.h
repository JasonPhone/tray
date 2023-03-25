#pragma once
#include "core/TRay.h"
#include "core/geometry/Interaction.h"
#include "core/geometry/Bound.h"
#include "core/geometry/Transform.h"

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
  virtual Bound3f object_bound() const = 0;
  virtual Bound3f world_bound() const {
    return obj_to_world(object_bound());
  }
  /**
   * @brief Get the first intersect in (0, t_max).
   *
   * @param ray Incoming ray.
   * @param time Return first hit time.
   * @param inter Return hit point interaction.
   * @param test_alpha_texture If test transparent texture.
   */
  virtual bool intersect(const Ray &ray, Float *time, SurfaceInteraction *si,
                         bool test_alpha_texture = true) const = 0;
  /// @brief Just test without getting any detailed info.
  virtual bool intersect_test(const Ray &ray,
                              bool test_alpha_texture = true) const {
    // Should use better method.
    Float t_hit = ray.t_max;
    SurfaceInteraction inter;
    return intersect(ray, &t_hit, &inter, test_alpha_texture);
  }
  virtual Float area() const = 0;
  const Transform &obj_to_world, &world_to_obj;
  const bool flip_normal;
  const bool swap_handness;
};
}  // namespace TRay