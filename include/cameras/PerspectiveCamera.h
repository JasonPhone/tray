#pragma once
#include "core/Camera.h"
namespace TRay {
class PerspectiveCamera : public ProjectiveCamera {
 public:
  /// @brief 
  /// @param cam_to_world How camera moves through time.
  /// @param screen 
  /// @param shutter_open 
  /// @param shutter_close 
  /// @param lens_r 
  /// @param focal_d 
  /// @param fov
  /// @param film 
  PerspectiveCamera(const AnimateTransform &cam_to_world,
                    const Bound2f &screen,
                    Float shutter_open, Float shutter_close, Float lens_r,
                    Float focal_d, Float fov, Film *film);
  /// @brief Get the perspective transform. Based on the camera space.
  /// @param fov Field of view in degree.
  Float ray_sample(const CameraSample &cam_sample, Ray *gen_ray) const override;

 private:
};
}  // namespace TRay