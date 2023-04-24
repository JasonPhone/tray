#include "core/Sampler.h"
#include "core/Camera.h"

namespace TRay {
Sampler::Sampler(int64_t sample_per_pixel) : m_spp(sample_per_pixel) {
  SInfo("Sampler:: Created sampler with" +
        string_format("\n\tspp %llu", sample_per_pixel));
}
/// @details Order of consuming samples:
///          2D for point on film,
///          1D for time,
///          2D for point on lens.
CameraSample Sampler::camera_sample(const Point2i &p) {
  CameraSample cs;
  cs.m_point_film = Point2f(p) + sample_2D();
  cs.m_time = sample_1D();
  cs.m_point_lens = sample_2D();
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
  // In splitting of MCM this means how many sub-samples?
  m_sample_1D_array.push_back(std::vector<Float>(n * m_spp));
}
void Sampler::request_2D_array(int n) {
  ASSERT(n == round(n));
  m_2D_array_sizes.push_back(n);
  m_sample_2D_array.push_back(std::vector<Point2f>(n * m_spp));
}
const Float *Sampler::get_1D_array(int n) {
  if (m_1D_array_offset >= m_1D_array_sizes.size()) return nullptr;
  ASSERT(m_1D_array_sizes[m_1D_array_offset] == n);
  ASSERT(m_idx_current_pixel_sample < m_spp);
  return &m_sample_1D_array[m_1D_array_offset++]
                           [m_idx_current_pixel_sample * n];
}
const Point2f *Sampler::get_2D_array(int n) {
  if (m_2D_array_offset >= m_2D_array_sizes.size()) return nullptr;
  ASSERT(m_2D_array_sizes[m_2D_array_offset] == n);
  ASSERT(m_idx_current_pixel_sample < m_spp);
  return &m_sample_2D_array[m_2D_array_offset++]
                           [m_idx_current_pixel_sample * n];
}

PixelSampler::PixelSampler(int64_t samples_per_pxiel, int sample_dims)
    : Sampler(samples_per_pxiel) {
  SInfo("PixelSampler:: Created pixel sampler with" +
        string_format("\n\tsample dims %d", sample_dims));
  for (int i = 0; i < sample_dims; i++) {
    m_sample_1D.push_back(std::vector<Float>(m_spp));
    m_sample_2D.push_back(std::vector<Point2f>(m_spp));
  }
}
bool PixelSampler::next_sample() {
  m_idx_current_1D = m_idx_current_2D = 0;
  return Sampler::next_sample();
}
bool PixelSampler::set_sample_index(int64_t idx) {
  m_idx_current_1D = m_idx_current_2D = 0;
  return Sampler::set_sample_index(idx);
}

Float PixelSampler::sample_1D() {
  if ((size_t)m_idx_current_1D < m_sample_1D.size()) {
    return m_sample_1D[m_idx_current_1D++][m_idx_current_pixel_sample];
  } else {
    SWarn(
        "PixelSampler::sample_1D: Dimension index out of range, "
        "using uniform distribution.");
    return m_rng.uniform_float();
  }
}
Point2f PixelSampler::sample_2D() {
  if ((size_t)m_idx_current_2D < m_sample_2D.size()) {
    return m_sample_2D[m_idx_current_2D++][m_idx_current_pixel_sample];
  } else {
    SWarn(
        "PixelSampler::sample_2D: Dimension index out of range, "
        "using uniform distribution.");
    return Point2f(m_rng.uniform_float(), m_rng.uniform_float());
  }
}

GlobalSampler::GlobalSampler(int64_t samples_per_pixel)
    : Sampler(samples_per_pixel) {}
void GlobalSampler::start_pixel(const Point2i &p) {
  Sampler::start_pixel(p);
  m_dimension = 0;
  m_global_idx_current_sample = global_index(0);
  m_idx_array_end_dim = m_idx_array_start_dim + m_sample_1D_array.size() +
                        2 * m_sample_2D_array.size();
  // Fill sample arrays from global sequences.
  for (size_t i = 0; i < m_1D_array_sizes.size(); i++) {
    int n_samples = m_1D_array_sizes[i] * m_spp;
    for (int j = 0; j < n_samples; j++) {
      int64_t idx = global_index(j);
      m_sample_1D_array[i][j] =
          value_by_dimension(idx, m_idx_array_start_dim + i);
    }
  }
  // 1D and 2D arrays consume the same sequence.
  int dim = m_idx_array_start_dim + m_1D_array_sizes.size();
  for (size_t i = 0; i < m_2D_array_sizes.size(); i++) {
    int n_samples = m_2D_array_sizes[i] * m_spp;
    for (int j = 0; j < n_samples; j++) {
      int64_t idx = global_index(j);
      m_sample_2D_array[i][j].x =
          value_by_dimension(idx, m_idx_array_start_dim + dim);
      m_sample_2D_array[i][j].y =
          value_by_dimension(idx, m_idx_array_start_dim + dim + 1);
    }
    dim += 2;
  }
  ASSERT(dim == m_idx_array_end_dim);
}
bool GlobalSampler::next_sample() {
  m_dimension = 0;
  m_global_idx_current_sample = global_index(m_idx_current_pixel_sample + 1);
  return Sampler::next_sample();
}
bool GlobalSampler::set_sample_index(int64_t idx) {
  m_dimension = 0;
  m_global_idx_current_sample = global_index(idx);
  return Sampler::set_sample_index(idx);
}

Float GlobalSampler::sample_1D() {
  // Skip dimensions for the arrays.
  if (m_idx_array_start_dim <= m_dimension && m_dimension < m_idx_array_end_dim)
    m_dimension = m_idx_array_end_dim;
  return value_by_dimension(m_global_idx_current_sample, m_dimension++);
}
Point2f GlobalSampler::sample_2D() {
  // Skip dimensions for the arrays.
  // m_dimension here is for first of the two.
  if (m_idx_array_start_dim <= m_dimension + 1 &&
      m_dimension < m_idx_array_end_dim)
    m_dimension = m_idx_array_end_dim;
  Float x = value_by_dimension(m_global_idx_current_sample, m_dimension);
  Float y = value_by_dimension(m_global_idx_current_sample, m_dimension + 1);
  m_dimension += 2;
  return Point2f(x, y);
}
}  // namespace TRay