#pragma once
#include "core/TRay.h"
#include "core/Sampler.h"

namespace TRay {
class StratifiedSampler : public PixelSampler {
 public:
  /// @brief
  /// @param x_num Number of samples in x axis (if 2D).
  /// @param y_num Number of samples in y axis (if 2D).
  /// @param jitter If the samples jitter or stay at center.
  StratifiedSampler(int x_num, int y_num, int num_dim, bool jitter);
  void start_pixel(const Point2i &p) override;
  std::unique_ptr<Sampler> clone(int seed) const override;

 private:
  // spp = x * y.
  const int m_x_samples, m_y_samples;
  // Do the samples need to jitter or stay at centers.
  const bool m_jitter;
};

}  // namespace TRay