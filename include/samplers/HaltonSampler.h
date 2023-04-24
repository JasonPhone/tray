#pragma once
#include "core/TRay.h"
#include "core/Sampler.h"

namespace TRay {
class HaltonSampler : public GlobalSampler {
 public:
  /// @param spp Sample per pixel.
  /// @param sample_bound Bound to be sampled, Film::sample_bound().
  HaltonSampler(int spp, const Bound2i &sample_bound);
  int64_t global_index(int64_t native_index) const override;
  Float value_by_dimension(int64_t global_idx_sample,
                                   int idx_dim) const override;
  std::unique_ptr<Sampler> clone(int seed) const override;

 private:
  // Samples on pixel position ranges in this extent.
  // The bigger will simply repeat.
  static constexpr int MAX_RESOLUTION = 128;
  Point2i m_scale_ceil;
  Point2i m_scale_expo;
  Point2i m_mult_inv;
  // Each position will be visited in this frequency.
  int m_sample_stride;
  // 
  mutable Point2i m_offset_pixel =
      Point2i(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
  mutable int64_t m_offset_global_idx;
};
}  // namespace TRay