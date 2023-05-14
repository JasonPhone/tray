#pragma once
#include "core/TRay.h"
#include "core/Sampler.h"
#include "core/geometry/Point.h"

namespace TRay {
class RandomSampler : public PixelSampler {
 public:
  RandomSampler(int64_t spp, int n_dims) : PixelSampler(spp, n_dims) {}
  Float sample_1D() override {
    return std::min(m_rng.uniform_float(), ONE_M_EPS);
  }
  Point2f sample_2D() override {
    Float x = std::min(m_rng.uniform_float(), ONE_M_EPS);
    Float y = std::min(m_rng.uniform_float(), ONE_M_EPS);
    return Point2f{x, y};
  }
  void start_pixel(const Point2i &p) override {
    // Fill 1D array.
    for (size_t i = 0; i < m_1D_array_sizes.size(); i++) {
      int n_subsamples = m_1D_array_sizes[i];
      for (int64_t si = 0; si < m_spp; si++) {
        Float *arr = &m_sample_1D_array[i][si * n_subsamples];
        for (int ssi = 0; ssi < n_subsamples; ssi++) {
          arr[ssi] = std::min(m_rng.uniform_float(), ONE_M_EPS);
        }
      }
    }
    // Fill 2D array.
    for (size_t i = 0; i < m_2D_array_sizes.size(); i++) {
      int n_subsamples = m_2D_array_sizes[i];
      for (int64_t si = 0; si < m_spp; si++) {
        Point2f *arr = &m_sample_2D_array[i][si * n_subsamples];
        for (int ssi = 0; ssi < n_subsamples; ssi++) {
          Float x = std::min(m_rng.uniform_float(), ONE_M_EPS);
          Float y = std::min(m_rng.uniform_float(), ONE_M_EPS);
          arr[ssi] = Point2f{x, y};
        }
      }
    }
    PixelSampler::start_pixel(p);
  }
  std::unique_ptr<Sampler> clone(int seed) const override {
    RandomSampler *ss = new RandomSampler(*this);
    ss->m_rng.set_sequence(seed);
    return std::unique_ptr<Sampler>(ss);
  }

 private:
};
}  // namespace TRay