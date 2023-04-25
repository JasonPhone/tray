#include "samplers/ZeroTwoSampler.h"
#include "core/math/lowdiscrepancy.h"

namespace TRay {
void ZeroTwoSampler::start_pixel(const Point2i &p) {
  // 1D
  for (size_t i = 0; i < m_sample_1D.size(); i++)
    fill_VDCorput_1D(1, m_spp, &m_sample_1D[i][0], m_rng);
  // 2D
  for (size_t i = 0; i < m_sample_2D.size(); i++)
    fill_Sobol_2D(1, m_spp, &m_sample_2D[i][0], m_rng);

  // 1D array
  for (size_t i = 0; i < m_1D_array_sizes.size(); i++)
    fill_VDCorput_1D(m_1D_array_sizes[i], m_spp, &m_sample_1D_array[i][0],
                     m_rng);
  // 2D array
  for (size_t i = 0; i < m_2D_array_sizes.size(); i++)
    fill_Sobol_2D(m_2D_array_sizes[i], m_spp, &m_sample_2D_array[i][0], m_rng);

  PixelSampler::start_pixel(p);
}
}  // namespace TRay