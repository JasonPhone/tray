#pragma once
#include "core/TRay.h"
#include "core/Camera.h"
#include "core/Film.h"
#include "core/math/sampling.h"

namespace TRay {
class OrthographicCamera : public ProjectiveCamera {
 public:
  OrthographicCamera(const Transform &cam_to_world, const Bound2f &screen,
                     Float shutter_open, Float shutter_close, Float lens_radius,
                     Float focal_distance, std::shared_ptr<Film> film)
      : ProjectiveCamera(cam_to_world, orthographic(0, 1), screen, shutter_open,
                         shutter_close, lens_radius, focal_distance, film) {
    SInfo("OrthographicCamera:: Created OrthographicCamera.");
  }

  Float ray_sample(const CameraSample &cam_sample,
                   Ray *gen_ray) const override {
    Point3f p_film =
        Point3f(cam_sample.m_point_film.x, cam_sample.m_point_film.y, 0);
    Point3f p_cam = m_raster_to_cam(p_film);
    *gen_ray = Ray(p_cam, Vector3f(0, 0, 1));
    // Thin lens approx.
    if (m_lens_radius > 0) {
      Point2f pLens =
          m_lens_radius * disk_concentric_sample(cam_sample.m_point_lens);
      Float ft = m_focal_distance / gen_ray->dir.z;
      Point3f p_focus = (*gen_ray)(ft);
      gen_ray->ori = Point3f(pLens.x, pLens.y, 0);
      gen_ray->dir = normalize(p_focus - gen_ray->ori);
    }
    gen_ray->time = lerp(cam_sample.m_time, m_shutter_open, m_shutter_close);
    *gen_ray = m_cam_to_world(*gen_ray);
    return 1;
  }

 private:
};
}  // namespace TRay