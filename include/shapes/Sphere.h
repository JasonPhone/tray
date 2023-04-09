#pragma once
#include "core/TRay.h"
#include "core/geometry/geometry.h"

namespace TRay {
class Sphere : public Shape {
 public:
  /**
   * @brief Construct a new Sphere object.
   *
   * @param obj_world Tranform from object space to world space.
   * @param world_obj Inversed.
   * @param flip_n true if the surface normal is flipped.
   * @param r Radius of the sphere.
   * @note Partial sphere not supported yet.
   */
  Sphere(const Transform &obj_world, const Transform &world_obj, bool flip_n,
         Float r)
      : Shape(obj_world, world_obj, flip_n),
        radius(r),
        z_min(-r),
        z_max(r),
        theta_min(0),
        theta_max(PI),
        phi_max(deg_to_rad(360)) {}
  Bound3f object_bound() const override;
  bool intersect(const Ray &ray, Float *time, SurfaceInteraction *si,
                 bool test_alpha_texture = true) const override;
  bool intersect_test(const Ray &ray,
                      bool test_alpha_texture = true) const override;
  Interaction sample_surface(const Point2f &u,
                             Float *pdf_value = nullptr) const override;
  Interaction sample_surface(const Interaction &ref, const Point2f &u,
                             Float *pdf_value = nullptr) const override;
  Float pdf(const Interaction &ref, const Vector3f &wi) const override;
  Float area() const override;
  /**
   * @brief Construct a new Sphere object.
   *
   * @param obj_world Tranform from object space to world space.
   * @param world_obj Inversed.
   * @param flip_n true if the surface normal is flipped.
   * @param r Radius of the sphere.
   * @note Partial sphere not supported yet.
   * @param z_min The lower plane truncating the sphere.
   * @param z_max The upper plane truncating the sphere.
   * @param p_max Sphere surface where phi above p_max is removed.
   */
  // Sphere(const Transform &obj_world, const Transform &world_obj, bool flip_n,
  //        Float r, Float z_min, Float z_max, Float p_max)
  //     : Shape(obj_world, world_obj, flip_n),
  //       radius(r),
  //       z_min(clamp(std::min(z_min, z_max), -r, r)),
  //       z_max(clamp(std::max(z_min, z_max), -r, r)),
  //       theta_min(std::acos(clamp(z_min / r, -1, 1))),
  //       theta_max(std::acos(clamp(z_max / r, -1, 1))),
  //       phi_max(deg_to_rad(clamp(p_max, 0, 360))) {}

 private:
  const Float radius;
  const Float z_min, z_max;
  const Float theta_min, theta_max;
  const Float phi_max;
};
}  // namespace TRay