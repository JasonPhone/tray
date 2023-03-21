#pragma once
#include "core/TRay.h"
#include "core/geometry/Point.h"
#include "core/geometry/Normal.h"

namespace TRay {

/// @brief Interaction interface decl.
/// ----------------------------------
class Interaction {
 public:
  Interaction() : time(0) {}
  Interaction(const Point3f &p, const Normal3f &n, const Vector3f &wo,
              Float time)
      : p(p), time(time), wo(normalize(wo)), n(n) {}
  bool IsSurfaceInteraction() const { return n != Normal3f(); }

  /// @brief  Point interection happens.
  Point3f p;
  /// @brief  Time interection happens.
  Float time;
  /// @brief  Float rounding error.
  // Vector3f p_error;
  /// @brief  Outgoing direction, (0, 0, 0) if no incoming ray.
  Vector3f wo;
  /// @brief Surface normal, (0, 0, 0) if not at surface.
  Normal3f n;
};

/// @brief SurfaceInteraction decl.
/// -------------------------------
class SurfaceInteraction : public Interaction {
 public:
  SurfaceInteraction() : Interaction() {}
  SurfaceInteraction(const Point3f &p, const Point2f &uv, const Vector3f &wo,
                     const Vector3f &dpdu, const Vector3f &dpdv, Float time,
                     const Shape *sh_ptr);
  // SurfaceInteraction(const Point3f &p, const Point2f &uv, const Vector3f &wo,
  //                    const Vector3f &dpdu, const Vector3f &dpdv,
  //                    const Normal3f &dndu, const Normal3f &dndv, Float time,
  //                    const Shape *sh_ptr, int faceIndex = 0);

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