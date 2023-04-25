#pragma once
#include "core/TRay.h"
#include "core/Sampler.h"

namespace TRay {
class SobolSampler : public GlobalSampler {
 public:
  SobolSampler(int64_t spp, const Bound2i &sample_bound)
      : GlobalSampler(round(spp)), m_sample_bound(sample_bound) {
    // Take the longest edge as scale factor.
    m_scale = pow2_ceil(
        std::max(sample_bound.diagonal().x, sample_bound.diagonal().y));
    m_scale_expo = log2_int(m_scale);
    SInfo(
        string_format("SobolSampler:: Created Sobol' sampler with"
                      "\n\tspp %d rounded up to %d"
                      "\n\tscale resolution %d",
                      spp, round(spp), m_scale));
  }
  int64_t global_index(int64_t local_index) const override;
  Float value_by_dimension(int64_t global_idx_sample,
                           int idx_dim) const override;
  std::unique_ptr<Sampler> clone(int seed) const {
    return std::unique_ptr<Sampler>(new SobolSampler(*this));
  }
  int round(int n) override { return pow2_ceil(n); }

 private:
  const Bound2i m_sample_bound;
  int m_scale, m_scale_expo;
};
}  // namespace TRay