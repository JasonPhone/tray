#include "samplers/MaxMinDisSampler.h"
namespace TRay {

void MaxMinDisSampler::start_pixel(const Point2i &p) {
  // 1D
  for (size_t i = 0; i < m_sample_1D.size(); i++)
    fill_VDCorput_1D(1, m_spp, &m_sample_1D[i][0], m_rng);
  // 2D
  Float spp_inv = 1.0 / m_spp;
  // Special for first dimension.
  for (int64_t i = 0; i < m_spp; i++)
    m_sample_2D[0][i] = Point2f{i * spp_inv, sample_generator_mat(m_C, i)};
  shuffle(&m_sample_2D[0][0], m_spp, 1, m_rng);
  for (size_t i = 1 /* Note this. */; i < m_sample_2D.size(); i++)
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