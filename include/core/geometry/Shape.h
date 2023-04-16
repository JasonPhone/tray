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
  virtual ~Shape() {}
  Shape(const Transform &obj_world, const Transform &world_obj, bool flip_n)
      : obj_to_world(std::make_shared<Transform>(obj_world)),
        world_to_obj(std::make_shared<Transform>(world_obj)),
        flip_normal(flip_n),
        swap_handness(obj_world.will_swap_hand()) {}
  virtual Bound3f object_bound() const = 0;
  virtual Bound3f world_bound() const {
    return (*obj_to_world)(object_bound());
  }
  /**
   * @brief Get the first intersect in (0, t_max).
   *
   * @param ray Incoming ray.
   * @param thit Return first hit parametric distance.
   * @param inter Return hit point interaction.
   * @param test_alpha_texture If test transparent texture.
   */
  virtual bool intersect(const Ray &ray, Float *thit, SurfaceInteraction *si,
                         bool test_alpha_texture = true) const = 0;
  /// @brief Just test without getting any detailed info.
  virtual bool intersect_test(const Ray &ray,
                              bool test_alpha_texture = true) const {
    // Should use better method.
    Float t;
    SurfaceInteraction si;
    return intersect(ray, &t, &si, test_alpha_texture);
  }
  /// @brief Sample a point on surface of the shape.
  /// @param u Uniform random values.
  /// @param pdf_value Store the pdf value.
  virtual Interaction sample_surface(const Point2f &u,
                                     Float *pdf_value) const = 0;
  /// @brief Sample a point on surface from a reference outside the shape.
  virtual Interaction sample_surface(const Interaction &ref, const Point2f &u,
                                     Float *pdf_value) const {
    // Default impl, shuold be overrided.
    return sample_surface(u, pdf_value);
  }
  virtual Float pdf(const Interaction &) const { return 1.0 / area(); }
  /// @brief Get pdf value of a ray, from @param ref and goes along @param wi.
  virtual Float pdf(const Interaction &ref, const Vector3f &wi) const {
    // Visibility test here considers only this shape.
    Ray ray = ref.ray_along(wi);
    Float thit;
    SurfaceInteraction si;
    if (!intersect(ray, &thit, &si, false)) return 0;
    // Transforming between area domain and direction domain.
    Float pdf_val = distance2(ref.p, si.p) / (abs_dot(si.n, -wi) * area());
    if (std::isinf(pdf_val)) pdf_val = 0.0;
    return pdf_val;
  }
  virtual Float area() const = 0;

  // const Transform obj_to_world, world_to_obj;
  std::shared_ptr<const Transform> obj_to_world, world_to_obj;
  const bool flip_normal;
  const bool swap_handness;
};
}  // namespace TRay