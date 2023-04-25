#include "samplers/SobolSampler.h"
#include "core/math/lowdiscrepancy.h"
#include "core/geometry/Point.h"

namespace TRay {

int64_t SobolSampler::global_index(int64_t local_index) const {
  Vector2i relative = m_current_pixel - m_sample_bound.p_min;
  return SobolIntervalToIndex(m_scale_expo, local_index,
                              Point2i(relative.x, relative.y));
}
Float SobolSampler::value_by_dimension(int64_t global_idx_sample,
                                       int idx_dim) const {
  Float value = sample_Sobol(global_idx_sample, idx_dim);
  // For pixel position.
  if (idx_dim == 0 || idx_dim == 1) {
    value = value * m_scale + m_sample_bound.p_min[idx_dim];
    value = clamp(value - m_current_pixel[idx_dim], Float(0), ONE_M_EPS);
  }
  return value;
}
}  // namespace TRay