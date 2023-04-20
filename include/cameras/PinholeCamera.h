#pragma once
#include "core/TRay.h"
#include "core/Camera.h"
#include "core/Film.h"

namespace TRay {
/// @brief A simple pinhole camera model.
class PinholeCamera : public Camera {
 public:
  /// @param cam_to_world
  /// @param fov Total field of view in horizontal (x) axis.
  PinholeCamera(const Transform &cam_to_world, Float shutter_open,
                Float shutter_close, Float fov, std::shared_ptr<Film> film)
      : Camera(cam_to_world, shutter_open, shutter_close, film) {
    m_film_distance =
        film->m_full_resolution.x * 0.5 / std::tan(deg_to_rad(fov) * 0.5);
    m_xoff = film->m_full_resolution.x * 0.5;
    m_yoff = film->m_full_resolution.y * 0.5;
    SInfo(
        string_format("PinholeCamera:: Created PinholeCamera with"
                      "\n\t fov %f"
                      "\n\t film distance %f",
                      fov, m_film_distance));
  }
  Float ray_sample(const CameraSample &cam_sample,
                   Ray *gen_ray) const override {
    const auto &p = cam_sample.m_point_film;
    if (gen_ray) {
      Vector3f dir = Vector3f{p.x - m_xoff, m_yoff - p.y, m_film_distance};
      *gen_ray = Ray{Point3f{0, 0, 0}, normalize(dir)};
      *gen_ray = m_cam_to_world(*gen_ray);
      gen_ray->time = lerp(m_shutter_open, m_shutter_close, cam_sample.m_time);
    }
    return 1.0;
  }

 private:
  Float m_film_distance;
  Float m_xoff, m_yoff;
};
}  // namespace TRay