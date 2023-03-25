#pragma once
#include "core/Camera.h"
namespace TRay {
class PerspectiveCamera : public ProjectiveCamera {
 public:
  PerspectiveCamera(const AnimateTransform &cam_to_world,
                    const Transform &cam_to_screen, const Bound2f &screen,
                    Float shutter_open, Float shutter_close, Float lens_r,
                    Float focal_d, Film *film);
  /// @brief Get the perspective transform. Based on the camera space.
  /// @param fov Field of view in degree.
  Transform perspective(Float fov, Float z_near, Float z_far);
  Float ray_sample(const CameraSample &cam_sample, Ray *gen_ray) const override;

 private:
};
}  // namespace TRay