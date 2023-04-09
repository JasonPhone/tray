#pragma once
#include "core/TRay.h"
#include "core/geometry/Transform.h"
#include "core/geometry/Vector.h"
// TODO Anti-aliasing needed.
namespace TRay {
/// @brief Mapping interface. To a 2D point.
class TextureMapping2D {
 public:
  virtual Point2f map(const SurfaceInteraction &si) const = 0;
};
/// @brief Mapping using uv coord of SurfaceInteraction.
class UVMapping2D : public TextureMapping2D {
 public:
  UVMapping2D(Float su, Float sv, Float offset_u, Float offset_v);
  Point2f map(const SurfaceInteraction &si) const override;

 private:
  const Float m_su, m_sv, m_offset_u, m_offset_v;
};
/// @brief Mapping using spherical theta and phi.
class SphericalMapping2D : public TextureMapping2D {
 public:
  SphericalMapping2D(const Transform &world_to_texture);
  Point2f map(const SurfaceInteraction &si) const override;

 private:
  Point2f onto_sphere(const Point3f &p) const;
  const Transform m_world_to_tex;
};
/// @brief Mapping using two Vector3f as basis.
class PlanarMapping2D : public TextureMapping2D {
 public:
  PlanarMapping2D(const Vector3f &s, const Vector3f &t, Float offset_s,
                  Float offset_t);
  Point2f map(const SurfaceInteraction &si) const override;

 private:
  Vector3f m_vs, m_vt;
  Float m_offset_s, m_offset_t;
};

/// @brief Mapping interface. To a 3D point.
class TextureMapping3D {
 public:
  virtual Point3f map(const SurfaceInteraction &si) const = 0;
};
/// @brief Mapping using Transform. Often just transform back to the obj space.
class TransformMapping3D : public TextureMapping3D {
 public:
  TransformMapping3D(const Transform &world_to_texture);
  Point3f map(const SurfaceInteraction &si) const override;

 private:
  const Transform m_world_to_tex;
};

/// @brief Texture function.
/// @tparam T Type of evaluated value.
template <typename T>
class Texture {
 public:
  virtual T evaluate(const SurfaceInteraction &si) const = 0;
  virtual ~Texture() {}
};
}  // namespace TRay