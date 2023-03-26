#include "core/Sampler.h"
#include "core/Camera.h"

namespace TRay {
Sampler::Sampler(int64_t sample_per_pixel) : m_spp(sample_per_pixel) {}
/// @details Order of consuming samples:
///          2D for point on film,
///          1D for time,
///          2D for point on lens.
CameraSample Sampler::camera_sample(const Point2i &p) {
  CameraSample cs;
  cs.m_point_on_film = Point2f(p) + sample_2D();
  cs.m_time = sample_1D();
  cs.m_point_on_lens = sample_2D();
  return cs;
}
void Sampler::start_pixel(const Point2i &p) {
  // Change the current pixel and sample index.
  m_current_pixel = p;
  m_idx_current_pixel_sample = 0;
  // Reset array offset.
  m_1D_array_offset = m_2D_array_offset = 0;
}
bool Sampler::next_sample() {
  // Reset array offset.
  m_1D_array_offset = m_2D_array_offset = 0;
  return ++m_idx_current_pixel_sample < m_spp;
}
bool Sampler::set_sample_index(int64_t idx) {
  // Reset array offset.
  m_1D_array_offset = m_2D_array_offset = 0;
  m_idx_current_pixel_sample = idx;
  return m_idx_current_pixel_sample < m_spp;
}
void Sampler::request_1D_array(int n) {
  ASSERT(n == round(n));
  m_1D_array_sizes.push_back(n);
  // Note the layout: first n values is for each sample.
  m_sample_1D_array.push_back(std::vector<Float>(n * m_spp));
}
void Sampler::request_2D_array(int n) {
  ASSERT(n == round(n));
  m_2D_array_sizes.push_back(n);
  m_sample_2D_array.push_back(std::vector<Point2f>(n * m_spp));
}
const Float *Sampler::get_1D_array(int n) {
  if (m_1D_array_offset >= m_1D_array_sizes.size()) return nullptr;
  ASSERT(m_1D_array_sizes[m_1D_array_offset] == 0);
  ASSERT(m_idx_current_pixel_sample < m_spp);
  return &m_sample_1D_array[m_1D_array_offset++][m_idx_current_pixel_sample * n];
}
}  // namespace TRay