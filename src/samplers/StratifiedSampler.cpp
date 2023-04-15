#include "core/Sampler.h"
#include "core/math/sampling.h"
#include "samplers/StratifiedSampler.h"

namespace TRay {

StratifiedSampler::StratifiedSampler(int x_num, int y_num, int num_dim,
                                     bool jitter)
    : PixelSampler(x_num * y_num, num_dim),
      m_x_samples(x_num),
      m_y_samples(y_num),
      m_jitter(jitter) {
  SInfo("StratifiedSampler:: Created stratified sampler" +
        string_format("\n\tx strata %d, y strata %d, jitter %d", m_x_samples,
                      m_y_samples, m_jitter));
}
void StratifiedSampler::start_pixel(const Point2i &p) {
  // Fill for sampling 1D.
  for (size_t i = 0; i < m_sample_1D.size(); i++) {
    fill_stratified_1D(&m_sample_1D[i][0], m_x_samples * m_y_samples, m_rng,
                       m_jitter);
    shuffle(&m_sample_1D[i][0], m_x_samples * m_y_samples, 1, m_rng);
  }
  // Fill for sampling 2D.
  for (size_t i = 0; i < m_sample_2D.size(); i++) {
    fill_stratified_2D(&m_sample_2D[i][0], m_x_samples, m_y_samples, m_rng,
                       m_jitter);
    shuffle(&m_sample_2D[i][0], m_x_samples * m_y_samples, 1, m_rng);
  }
  // Fill for sampling 1D array.
  for (size_t i = 0; i < m_1D_array_sizes.size(); i++) {
    int n = m_1D_array_sizes[i];
    for (int64_t j = 0; j < m_spp; j++) {
      fill_stratified_1D(&m_sample_1D_array[i][n * j], n, m_rng, m_jitter);
      shuffle(&m_sample_1D_array[i][n * j], n, 1, m_rng);
    }
  }
  // Fill for sampling 2D array.
  Float spp_inv = 1.0 / m_spp;
  for (size_t offset = 0; offset < m_2D_array_sizes.size(); offset++) {
    int n = m_2D_array_sizes[offset];
    // Fill LHS values.
    for (int64_t spp_idx = 0; spp_idx < m_spp; spp_idx++) {
      Float diagonal_val = (spp_idx + m_rng.uniform_float()) * spp_inv;
      diagonal_val = std::min(diagonal_val, ONE_M_EPS);
      for (int dim_idx = 0; dim_idx < n; dim_idx++) {
        m_sample_2D_array[offset][m_spp * dim_idx + spp_idx].x = diagonal_val;
        m_sample_2D_array[offset][m_spp * dim_idx + spp_idx].y = diagonal_val;
      }
    }
    // Shuffle LHS values. By dimension and break each tuple.
    for (int dim_idx = 0; dim_idx < n; dim_idx++) {
      Point2f *arr = &m_sample_2D_array[offset][dim_idx * m_spp];
      for (int64_t spp_idx = 0; spp_idx < m_spp; spp_idx++) {
        int other = spp_idx + m_rng.uniform_uint32(m_spp - spp_idx);
        std::swap(arr[spp_idx].x, arr[other].x);
      }
      for (int64_t spp_idx = 0; spp_idx < m_spp; spp_idx++) {
        int other = spp_idx + m_rng.uniform_uint32(m_spp - spp_idx);
        std::swap(arr[spp_idx].y, arr[other].y);
      }
    }
  }
  PixelSampler::start_pixel(p);
}
std::unique_ptr<Sampler> StratifiedSampler::clone(int seed) const {
  StratifiedSampler *ss = new StratifiedSampler(*this);
  ss->m_rng.set_sequence(seed);
  return std::unique_ptr<Sampler>(ss);
}
}  // namespace TRay