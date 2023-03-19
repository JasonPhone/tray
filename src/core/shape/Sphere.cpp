#include "core/shape/Sphere.h"
#include "core/geometry/Point.h"

namespace TRay {
Bound3f Sphere::do_object_bound() const {
  return Bound3f(Point3f(-radius, -radius, z_min),
                 Point3f(radius, radius, z_max));
}
bool Sphere::do_intersect(const Ray &ray, Float *time,
                          SurfaceInteraction *si,
                          bool test_alpha_texture = true) const {
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
  if (p_hit.x == 0 && p_hit.y == 0) p_hit.x *= 1e-5f * radius;
  phi = std::atan2(p_hit.y, p_hit.x);
  if (phi < 0) phi += 2 * PI;
  Float theta = std::acos(clamp(p_hit.z, -1, 1));
  // Calculate uv.
  Float u = phi / phi_max;
  Float v = (theta - theta_min) / (theta_max - theta_min);
  // Compute sphere dpdu and dpdv
  Float z_radius = std::sqrt(p_hit.x * p_hit.x + p_hit.y * p_hit.y);
  Float z_radius_inv = 1 / z_radius;
  Float cosPhi = p_hit.x * z_radius_inv;
  Float sinPhi = p_hit.y * z_radius_inv;
  Vector3f dpdu(-phi_max * p_hit.y, phi_max * p_hit.x, 0);
  Vector3f dpdv =
      (theta_max - theta_min) *
      Vector3f(p_hit.z * cosPhi, p_hit.z * sinPhi, -radius * std::sin(theta));
  // Return values.
  *time = t_hit;
  *si = obj_to_world(SurfaceInteraction(p_hit, Point2f(u, v), -ray.dir, dpdu,
                                           dpdv, ray.time, this));
  return true;
}
bool Sphere::do_intersect_test(const Ray &ray,
                               bool test_alpha_texture = true) const {
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
  if (p_hit.x == 0 && p_hit.y == 0) p_hit.x *= 1e-5f * radius;
  phi = std::atan2(p_hit.y, p_hit.x);
  if (phi < 0) phi += 2 * PI;
  Float theta = std::acos(clamp(p_hit.z, -1, 1));
  // Calculate uv.
  Float u = phi / phi_max;
  Float v = (theta - theta_min) / (theta_max - theta_min);
  // Compute sphere dpdu and dpdv
  Float z_radius = std::sqrt(p_hit.x * p_hit.x + p_hit.y * p_hit.y);
  Float z_radius_inv = 1 / z_radius;
  Float cosPhi = p_hit.x * z_radius_inv;
  Float sinPhi = p_hit.y * z_radius_inv;
  Vector3f dpdu(-phi_max * p_hit.y, phi_max * p_hit.x, 0);
  Vector3f dpdv =
      (theta_max - theta_min) *
      Vector3f(p_hit.z * cosPhi, p_hit.z * sinPhi, -radius * std::sin(theta));

  return true;
}
Float Sphere::do_area() const {
  return phi_max * radius * (z_max - z_min);
}
}  // namespace TRay