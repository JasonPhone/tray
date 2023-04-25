#include "samplers/HaltonSampler.h"
#include "core/math/lowdiscrepancy.h"

namespace TRay {
/// @brief Solve x, y for ax + by = 1.
static void exgcd(uint64_t a, uint64_t b, int64_t *x, int64_t *y) {
  if (b == 0) {
    *x = 1;
    *y = 0;
    return;
  }
  int64_t d = a / b, xp, yp;
  exgcd(b, a % b, &xp, &yp);
  *x = xp;
  *y = xp - d * yp;
}
/// @brief Get the multiplicative inverse of a (mod n).
///        That is, (a * x) % n == 1.
static uint64_t multiply_inverse(int64_t a, int64_t n) {
  // a * x = 1 + n * y --> a * x - n * y = 1 --> a * X + b * Y = 1
  int64_t x, y;
  exgcd(a, n, &x, &y);
  return mod(x, n);
}
HaltonSampler::HaltonSampler(int spp, const Bound2i &sample_bound)
    : GlobalSampler(spp) {
  // Scaling to map the first 2 dims to pixel position.
  Vector2i reso = sample_bound.p_max - sample_bound.p_min;
  for (int i = 0; i < 2; i++) {
    int base = (i == 0 ? 2 : 3);
    int extent = 1, expo = 0;
    while (extent < std::min(reso[i], MAX_RESOLUTION)) {
      extent *= base;
      expo++;
    }
    m_scale_ceil[i] = extent;
    m_scale_expo[i] = expo;
  }
  // Samples' position stride for first 2 dims.
  m_sample_stride = m_scale_ceil[0] * m_scale_ceil[1];
  // Multiplicetive inverse for determine the first global index using CRT.
  m_mult_inv[0] = multiply_inverse(m_scale_ceil[1], m_scale_ceil[0]);
  m_mult_inv[1] = multiply_inverse(m_scale_ceil[0], m_scale_ceil[1]);
  SInfo(
      string_format("HaltonSampler:: Created halton sampler with"
                    "\n\tspp %d"
                    "\n\tsample bound %s"
                    "\n\tscale %d, %d (stride %d)",
                    spp, sample_bound.to_string().c_str(), m_scale_ceil[0],
                    m_scale_ceil[1], m_sample_stride));
}
int64_t HaltonSampler::global_index(int64_t local_index) const {
  if (m_current_pixel != m_offset_pixel) {
    /**
     * X = i mod 2^m_scale_expo[0]
     * Y = i mod 3^m_scale_expo[1]
     * Use CRT to find the first solution.
     */
    m_offset_global_idx = 0;
    if (m_sample_stride > 1) {
      Point2i pm(mod(m_current_pixel[0], MAX_RESOLUTION),
                 mod(m_current_pixel[1], MAX_RESOLUTION));
      for (int i = 0; i < 2; ++i) {
        uint64_t offset_in_dim =
            (i == 0) ? radical_inverse_inv<2>(pm[i], m_scale_expo[i])
                     : radical_inverse_inv<3>(pm[i], m_scale_expo[i]);
        m_offset_global_idx +=
            offset_in_dim * (m_sample_stride / m_scale_ceil[i]) * m_mult_inv[i];
      }
      m_offset_global_idx %= m_sample_stride;
    }
    m_offset_pixel = m_current_pixel;
  }
  return m_offset_global_idx + local_index * m_sample_stride;
}
Float HaltonSampler::value_by_dimension(int64_t global_idx_sample,
                                        int idx_dim) const {
  Float ret = 0;
  if (idx_dim == 0 || idx_dim == 1)
    // Remove position-related digits and return the relative coord in a pixel.
    ret = radical_inverse(idx_dim, global_idx_sample / m_scale_ceil[idx_dim]);
  else
    ret = radical_inverse(idx_dim, global_idx_sample);
  return ret;
}

std::unique_ptr<Sampler> HaltonSampler::clone(int seed) const {
  return std::unique_ptr<Sampler>(new HaltonSampler{*this});
}
}  // namespace TRay