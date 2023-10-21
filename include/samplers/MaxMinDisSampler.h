#pragma once
#include "core/Sampler.h"
#include "core/math/lowdiscrepancy.h"

namespace TRay {
class MaxMinDisSampler : public PixelSampler {
 public:
  MaxMinDisSampler(int64_t spp, int n_dims) : PixelSampler(round(spp), n_dims) {
    // SInfo(string_format(
    //     "MaxMinSampler:: Created max-minimum distance sampler with"
    //     "\n\tspp %d rounded up to %d"
    //     "\n\tsample dimensions %d",
    //     spp, round(spp), n_dims));
    //     m_C = CMaxMinDis[log2_int(spp)];
  }
  void start_pixel(const Point2i &p) override;
  std::unique_ptr<Sampler> clone(int seed) const override {
    MaxMinDisSampler *sampler = new MaxMinDisSampler(*this);
    sampler->m_rng.set_sequence(seed);
    return std::unique_ptr<Sampler>(sampler);
  }
  int round(int n) override { return pow2_ceil(n); }

 private:
  const uint32_t *m_C;
};
}  // namespace TRay