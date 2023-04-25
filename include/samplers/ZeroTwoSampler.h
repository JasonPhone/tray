#pragma once
#include "core/TRay.h"
#include "core/Sampler.h"

namespace TRay {
/// @brief (0, 2)-sequence for 2D samples,
/// van der Corput sequence for 1D samples.
class ZeroTwoSampler : public PixelSampler {
 public:
  ZeroTwoSampler(int64_t spp, int n_dims) : PixelSampler(round(spp), n_dims) {
    SInfo(
        string_format("ZeroTwoSampler:: Created (0-2)-sequence sampler with"
                      "\n\tspp %d rounded up to %d"
                      "\n\tsample dimensions %d",
                      spp, round(spp), n_dims));
  }

  void start_pixel(const Point2i &p) override;
  std::unique_ptr<Sampler> clone(int seed) const override {
    ZeroTwoSampler *sampler = new ZeroTwoSampler(*this);
    sampler->m_rng.set_sequence(seed);
    return std::unique_ptr<Sampler>(sampler);
  }
  int round(int n) override { return pow2_ceil(n); }
};
}  // namespace TRay