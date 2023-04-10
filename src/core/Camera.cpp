#include "core/Camera.h"
#include "core/Film.h"
#include "core/Filter.h"

namespace TRay {
Camera::Camera(const AnimateTransform &cam_to_world, Float shutter_open,
               Float shutter_close, Film *film)
    : m_cam_to_world(cam_to_world),
      m_shutter_open(shutter_open),
      m_shutter_close(shutter_close),
      m_film(film) {}

ProjectiveCamera::ProjectiveCamera(const AnimateTransform &cam_to_world,
                                   const Transform &cam_to_screen,
                                   const Bound2f &screen, Float shutter_open,
                                   Float shutter_close, Float lens_r,
                                   Float focal_d, Film *film)
    : Camera(cam_to_world, shutter_open, shutter_close, film),
      m_cam_to_screen(cam_to_screen),
      m_lens_radius(lens_r),
      m_focal_distance(focal_d) {
  // Transformations. Read from right to left.
  m_screen_to_raster =
      // Scale to full size.
      scale(film->m_full_resolution.x, film->m_full_resolution.y, 1.0) *
      // Normalize and swap hand.
      scale(1.0 / (screen.p_max.x - screen.p_min.x),
            1.0 / (screen.p_min.y - screen.p_max.y), 1.0) *
      // Upper left corner.
      translate(Vector3f(-screen.p_min.x, -screen.p_max.y, 0));
  m_raster_to_screen = m_screen_to_raster.inverse();
  m_raster_to_cam = m_cam_to_screen.inverse() * m_raster_to_screen;
}
}  // namespace TRay
