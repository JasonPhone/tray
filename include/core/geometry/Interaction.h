#pragma once
#include "core/TRay.h"
#include "core/geometry/Point.h"
#include "core/geometry/Ray.h"
#include "core/geometry/Normal.h"
#include "core/Material.h"

namespace TRay {

class Interaction {
 public:
  Interaction() : time(0) {}
  Interaction(const Point3f &p, const Normal3f &n, const Vector3f &wo,
              Float time)
      : p(p), time(time), wo(normalize(wo)), n(n) {}
  Interaction(const Point3f &p, const Vector3f &wo, Float time)
      : p(p), time(time), wo(wo) {}
  Interaction(const Point3f &p, Float time) : p(p), time(time) {}
  bool is_surface_interaction() const { return n != Normal3f(); }
  Ray ray_along(const Vector3f &d) const { return Ray(p, d, time, TRAY_INF); }
  Ray ray_to(const Point3f &p2) const {
    Vector3f d = p2 - p;
    return Ray(p, d, time, 1 - TRAY_EPS);
  }
  Ray ray_to(const Interaction &it) const {
    return Ray(p, it.p - p, time, 1 - TRAY_EPS);
  }

  /// @brief  Point interection happens.
  Point3f p;
  /// @brief  Time interection happens.
  Float time;
  /// @brief  Outgoing direction, (0, 0, 0) if no incoming ray.
  Vector3f wo;
  /// @brief Surface normal, (0, 0, 0) if not at surface.
  Normal3f n;
};

class SurfaceInteraction : public Interaction {
 public:
  SurfaceInteraction() : Interaction() {}
  SurfaceInteraction(const Point3f &p, const Point2f &uv, const Vector3f &wo,
                     const Vector3f &dpdu, const Vector3f &dpdv, Float time,
                     const Shape *sh_ptr);
  void fill_scattering_func(const Ray &ray,
                            TransportMode mode = TransportMode::Radiance,
                            bool allow_multi_lobes = false);
  /// @brief Get the emitted radiance of area light
  ///        if this primitive is bound
  /// with an area light.
  /// @param w Light outgoing direction.
  Spectrum Le(const Vector3f &w) const;

  Point2f uv;
  /// @brief Partial derivatives.
  Vector3f dpdu, dpdv;
  // Normal3f dndu, dndv;
  /// @brief Ptr to corresponding Shape class.
  const Shape *shape = nullptr;
  /// @brief Perturbed or generated data, not original of the surface.
  struct {
    Normal3f n;
    Vector3f dpdu, dpdv;
    // Normal3f dndu, dndv;
  } shading;
  const Primitive *primitive = nullptr;
  BSDF *bsdf = nullptr;
  // BSSRDF *bssrdf = nullptr;
  // mutable Vector3f dpdx, dpdy;
  // mutable Float dudx = 0, dvdx = 0, dudy = 0, dvdy = 0;
};

}  // namespace TRay