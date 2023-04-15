#include "core/geometry/geometry.h"
#include "shapes/Sphere.h"
#include "core/math/sampling.h"

namespace TRay {
Bound3f Sphere::object_bound() const {
  return Bound3f(Point3f(-radius, -radius, z_min),
                 Point3f(radius, radius, z_max));
}
/**
 * Some points in doing intersection:
 *  1. Intersection after ray.t_max is ignored.
 *  2. Use *t to return parametric distance of the FIRST hit.
 *  3. Interaction is used widely to separate geometry code and shading code.
 *  4. Ray is in world space, but intersection test is easier in object space,
 *     and the return interaction info should be in world space.
 */
bool Sphere::intersect(const Ray &ray, Float *t, SurfaceInteraction *si,
                       bool test_alpha_texture) const {
  // SDebug("Sphere::intersect: testing ray " + ray.to_string());
  // SDebug("\twith sphere at " + obj_to_world(Point3f(0, 0, 0)).to_string() +
  //        ", radius " + format_one("%f ", radius));
  Float phi;
  Point3f p_hit;
  // Ray to obj space.
  Ray obj_ray = world_to_obj(ray);
  // Construct formula.
  Float dx = obj_ray.dir.x, dy = obj_ray.dir.y, dz = obj_ray.dir.z;
  Float ox = obj_ray.ori.x, oy = obj_ray.ori.y, oz = obj_ray.ori.z;
  Float a = dx * dx + dy * dy + dz * dz;
  Float b = 2 * (dx * ox + dy * oy + dz * oz);
  Float c = ox * ox + oy * oy + oz * oz - radius * radius;
  // Solve formula.
  Float t0, t1;
  if (!solve_quadratic(a, b, c, &t0, &t1)) return false;
  // The time range.
  if (t0 > obj_ray.t_max || t1 <= 0) return false;
  Float t_hit = t0;
  if (t_hit <= 0) {
    t_hit = t1;
    if (t_hit > obj_ray.t_max) return false;
  }
  p_hit = obj_ray(t_hit);
  // Refine.
  p_hit *= radius / distance(p_hit, Point3f(0, 0, 0));
  // Avoid 0 in following z_radius.
  if (p_hit.x == 0 && p_hit.y == 0) p_hit.x = 1e-5 * radius;
  phi = std::atan2(p_hit.y, p_hit.x);
  if (phi < 0) phi += 2 * PI;
  Float theta = std::acos(clamp(p_hit.z / radius, -1, 1));
  // Calculate uv.
  Float u = phi / phi_max;
  // Float v = (theta - theta_min) / (theta_max - theta_min);
  Float v = (theta - theta_max) / (theta_min - theta_max);
  // Compute sphere dpdu and dpdv
  Float z_radius = std::sqrt(p_hit.x * p_hit.x + p_hit.y * p_hit.y);
  Float z_radius_inv = 1 / z_radius;
  Float cos_phi = p_hit.x * z_radius_inv;
  Float sin_phi = p_hit.y * z_radius_inv;
  Vector3f dpdu(-phi_max * p_hit.y, phi_max * p_hit.x, 0);
  Vector3f dpdv =
      // (theta_max - theta_min) *
      (theta_min - theta_max) *
      Vector3f(p_hit.z * cos_phi, p_hit.z * sin_phi, -radius * std::sin(theta));
  // Return values.
  if (t) *t = t_hit;
  if (si)
    *si = obj_to_world(SurfaceInteraction(p_hit, Point2f(u, v), -ray.dir, dpdu,
                                          dpdv, ray.time, this));
  return true;
}
/// @brief Same as intersect() but does not update interaction.
bool Sphere::intersect_test(const Ray &ray, bool test_alpha_texture) const {
  SurfaceInteraction si;
  Float time;
  return this->intersect(ray, &time, &si, test_alpha_texture);
  // Float phi;
  // Point3f p_hit;
  // // Ray to obj space.
  // Ray obj_ray = world_to_obj(ray);
  // // Construct formula.
  // Float dx = obj_ray.dir.x, dy = obj_ray.dir.y, dz = obj_ray.dir.z;
  // Float ox = obj_ray.ori.x, oy = obj_ray.ori.y, oz = obj_ray.ori.z;
  // Float a = dx * dx + dy * dy + dz * dz;
  // Float b = 2 * (dx * ox + dy * oy + dz * oz);
  // Float c = ox * ox + oy * oy + oz * oz - radius * radius;
  // // Solve formula.
  // Float t0, t1;
  // if (!solve_quadratic(a, b, c, &t0, &t1)) return false;
  // // The time range.
  // if (t0 > obj_ray.t_max || t1 <= 0) return false;
  // Float t_hit = t0;
  // if (t_hit <= 0) {
  //   t_hit = t1;
  //   if (t_hit > obj_ray.t_max) return false;
  // }
  // p_hit = obj_ray(t_hit);
  // // Refine.
  // p_hit *= radius / distance(p_hit, Point3f(0, 0, 0));
  // if (p_hit.x == 0 && p_hit.y == 0) p_hit.x = 1e-5f * radius;
  // phi = std::atan2(p_hit.y, p_hit.x);
  // if (phi < 0) phi += 2 * PI;
  // Float theta = std::acos(clamp(p_hit.z, -1, 1));
  // // Calculate uv.
  // Float u = phi / phi_max;
  // Float v = (theta - theta_max) / (theta_min - theta_max);
  // // Compute sphere dpdu and dpdv
  // Float z_radius = std::sqrt(p_hit.x * p_hit.x + p_hit.y * p_hit.y);
  // Float z_radius_inv = 1 / z_radius;
  // Float cosPhi = p_hit.x * z_radius_inv;
  // Float sinPhi = p_hit.y * z_radius_inv;
  // Vector3f dpdu(-phi_max * p_hit.y, phi_max * p_hit.x, 0);
  // Vector3f dpdv =
  //     (theta_min - theta_max) *
  //     Vector3f(p_hit.z * cosPhi, p_hit.z * sinPhi, -radius *
  //     std::sin(theta));
  // return true;
}
Interaction Sphere::sample_surface(const Point2f &u, Float *pdf_value) const {
  Point3f p_surface = Point3f(0, 0, 0) + radius * sphere_uniform_sample(u);
  Interaction inter;
  inter.n =
      normalize(obj_to_world(Normal3f(p_surface.x, p_surface.y, p_surface.z)));
  if (flip_normal) inter.n *= -1;
  inter.p = obj_to_world(p_surface);
  if (pdf_value) *pdf_value = 1.0 / area();
  return inter;
}

Interaction Sphere::sample_surface(const Interaction &ref, const Point2f &u,
                                   Float *pdf_value) const {
  // Coord frame for sampling.
  Point3f p_center = obj_to_world(Point3f(0, 0, 0));
  Vector3f fz = normalize(p_center - ref.p);
  Vector3f fx, fy;
  make_coord_system(fz, &fx, &fy);
  if (distance2(ref.p, p_center) <= radius * radius) {
    // Inside the sphere, sample all.
    return sample_surface(u, pdf_value);
  }
  // Outside the sphere, sample the cone.
  // theta and phi from the sample space.
  Float sin2_theta_range = radius * radius / distance2(ref.p, p_center);
  Float cos_theta_range = std::sqrt(std::max((Float)0, 1 - sin2_theta_range));
  Float sample_cos_theta = (1 - u[0]) + u[0] * cos_theta_range;
  Float sample_sin_theta =
      std::sqrt(std::max((Float)0, 1 - sample_cos_theta * sample_cos_theta));
  Float sample_phi = u[1] * 2 * PI;
  // alpha the center angle of sphere.
  Float dc = distance(ref.p, p_center);
  Float ds = dc * sample_cos_theta -
             std::sqrt(std::max(
                 (Float)0, radius * radius -
                               dc * dc * sample_sin_theta * sample_sin_theta));
  Float cos_alpha = (dc * dc + radius * radius - ds * ds) / (2 * dc * radius);
  Float sin_alpha = std::sqrt(std::max((Float)0, 1 - cos_alpha * cos_alpha));
  // Sample point and normal in world space.
  Vector3f n_world =
      spherical_direction(sin_alpha, cos_alpha, sample_phi, -fx, -fy, -fz);
  Point3f p_world =
      p_center + radius * Point3f(n_world.x, n_world.y, n_world.z);
  // Construct the interaction.
  Interaction inter;
  inter.p = p_world;
  inter.n = Normal3f(n_world);
  if (flip_normal) inter.n *= -1;
  if (pdf_value) *pdf_value = cone_uniform_pdf(cos_theta_range);
  return inter;
}
Float Sphere::pdf(const Interaction &ref, const Vector3f &wi) const {
  Point3f p_center = obj_to_world(Point3f(0, 0, 0));
  // If inside the sphere.
  if (distance2(ref.p, p_center) <= radius * radius) return Shape::pdf(ref, wi);
  // Outside the sphere, cone uniform sampling.
  Float sin2_theta_range = radius * radius / distance2(ref.p, p_center);
  Float cos_theta_range = std::sqrt(std::max((Float)0, 1 - sin2_theta_range));
  return cone_uniform_pdf(cos_theta_range);
}
Float Sphere::area() const { return phi_max * radius * (z_max - z_min); }
}  // namespace TRay