#include "cameras/PerspectiveCamera.h"
#include "core/math/sampling.h"
#include "core/stringformat.h"

namespace TRay {
PerspectiveCamera::PerspectiveCamera(const AnimateTransform &cam_to_world,
                                     const Bound2f &screen, Float shutter_open,
                                     Float shutter_close, Float lens_r,
                                     Float focal_d, Float fov, Film *film)
    : ProjectiveCamera(cam_to_world, perspective(fov, 0.01, 1000.0), screen,
                       shutter_open, shutter_close, lens_r, focal_d, film) {
  SInfo("PerspectiveCamera:: Created perspective camera \n\t" + to_string());
}
Float PerspectiveCamera::ray_sample(const CameraSample &cam_sample,
                                    Ray *gen_ray) const {
  // From origin directly to point on near plane.
  Point3f p_film =
      Point3f(cam_sample.m_point_film.x, cam_sample.m_point_film.y, 0);
  Point3f p_camera = m_raster_to_cam(p_film);
  // PEEK(p_film.to_string());
  // PEEK(p_camera.to_string());
  *gen_ray = Ray(Point3f(0, 0, 0), normalize(Vector3f(p_camera)));
  // Thin lens approx.
  if (m_lens_radius > 0) {
    Point2f p_on_lens =
        m_lens_radius * disk_concentric_sample(cam_sample.m_point_lens);
    Float focus_time = m_focal_distance / gen_ray->dir.z;
    Point3f focus_pos = (*gen_ray)(focus_time);
    // The real origin and directon of this ray.
    gen_ray->ori = Point3f(p_on_lens.x, p_on_lens.y, 0);
    gen_ray->dir = normalize(focus_pos - gen_ray->ori);
  }
  gen_ray->time = lerp(m_shutter_open, m_shutter_close, cam_sample.m_time);
  *gen_ray = m_cam_to_world(*gen_ray);
  return 1.0;
}
}  // namespace TRay