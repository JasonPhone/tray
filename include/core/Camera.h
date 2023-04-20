#pragma once
#include "core/TRay.h"
#include "core/geometry/Transform.h"
#include "core/geometry/Point.h"
#include "core/geometry/Bound.h"

/**
 * Object space: Space in which geometry primitives are defined.
 * Camera space: Camera as origin, view direction as z+, up direction as y+.
 *               Handy for clipping.
 * World space: Base frame for all spaces.
 * Screen space: Defined on the film plane. The visible part of image. z (depth)
 *               in this space is in [0, 1] for near and far clipping planes.
 *               Yes this is still a 3D frame.
 * Normalized device space: NDC, a space on [0, 1]^3. This is mainly for
 *                          image resolution issues, and (0, 0, 0) is the
 *                          UPPER LEFT corner of it. Converted from screen
 *                          space by a linear transform.
 * Raster space: x and y are scaled from [0, 1]^2 in NDC to right resolutions.
 */
namespace TRay {
struct CameraSample {
  // Sample point on film, in [0, 1)^2.
  Point2f m_point_film;
  // Sample point on lens, in [0, 1)^2.
  Point2f m_point_lens;
  // Sample point on time, in [0, 1).
  Float m_time;
};
/// @brief All camera types are based on Camera.
class Camera {
 public:
  virtual ~Camera() {}
  Camera(const Transform &cam_to_world, Float shutter_open, Float shutter_close,
         std::shared_ptr<Film> film);
  /// @brief Get a ray based on the camera sample info.
  /// @param cam_sample Camera sample info.
  /// @param gen_ray For return value.
  /// @return The weight this ray contribute to the final image.
  virtual Float ray_sample(const CameraSample &cam_sample,
                           Ray *gen_ray) const = 0;
  virtual std::string to_string() const {
    return string_format(" shutter time [%f, %f]\n\t", m_shutter_open,
                         m_shutter_close);
  }
  Transform m_cam_to_world;
  const Float m_shutter_open, m_shutter_close;
  std::shared_ptr<Film> m_film;
};

/// @brief Cameras that uses a 4x4 projective matrix.
class ProjectiveCamera : public Camera {
 public:
  ProjectiveCamera(const Transform &cam_to_world,
                   const Transform &cam_to_screen, const Bound2f &screen,
                   Float shutter_open, Float shutter_close, Float lens_r,
                   Float focal_d, std::shared_ptr<Film> film);
  std::string to_string() const override {
    std::string str = Camera::to_string();
    str += string_format("lens radius %f, focal distance %f", m_lens_radius,
                         m_focal_distance);
    return str;
  }

 protected:
  Transform m_raster_to_cam, m_cam_to_screen;
  Transform m_raster_to_screen, m_screen_to_raster;
  Float m_lens_radius, m_focal_distance;
};
}  // namespace TRay