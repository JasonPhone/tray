#pragma once
#include "core/Camera.h"
namespace TRay {
class PerspectiveCamera : public ProjectiveCamera {
 public:
  /// @brief
  /// @param cam_to_world How camera moves through time.
  /// @param screen Cropped screen.
  /// @param shutter_open Shutter open time.
  /// @param shutter_close Shutter close time.
  /// @param lens_r Lens radius.
  /// @param focal_d Focal distance.
  /// @param fov Field of view, in degree, total angle.
  /// @param film Film to write.
  PerspectiveCamera(const Transform &cam_to_world, const Bound2f &screen,
                    Float shutter_open, Float shutter_close, Float lens_r,
                    Float focal_d, Float fov, std::shared_ptr<Film> film);
  Float ray_sample(const CameraSample &cam_sample, Ray *gen_ray) const override;
  std::string to_string() const override {
    std::string str = ProjectiveCamera::to_string();
    return str;
  }

 private:

};
}  // namespace TRay