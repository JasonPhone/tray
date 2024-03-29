#pragma once
#include "core/TRay.h"
#include "core/geometry/Point.h"
#include "core/math/RNG.h"

namespace TRay {
// Random sample functions.
// ------------------------
/// @brief Simple rejection sampling.
Point2f disk_rejection_sample(RNG &rng);
/// @brief Mapping a [0, 1)^2 point to 2D disk by inversion method.
/// @param u 2D uniform distribution over [0, 1)^2.
Point2f disk_uniform_sample(const Point2f &u);
/// @brief Mapping a [0, 1)^2 point to 2D disk by concentric mapping.
/// @param u 2D uniform distribution over [0, 1)^2.
Point2f disk_concentric_sample(const Point2f &u);
/// @brief Sample a 2D disk using a uniform RNG in [0, 1)^2.
/// @param rng Random number generator.
Float disk_uniform_pdf();
Vector3f hemisphere_uniform_sample(const Point2f &u);
Float hemisphere_uniform_pdf();
/// @brief Sample a point on (0, 0, 1) hemishpere with cos(theta) distribution.
Vector3f hemisphere_cosine_sample(const Point2f &u);
Float hemisphere_cosine_pdf(Float cos_theta);
Vector3f sphere_uniform_sample(const Point2f &u);
/// @brief Pdf value sampling UNIT sphere.
Float sphere_uniform_pdf();
/// @brief Uniform sample a cone of directions around (0, 0, 1).
/// @param cos_theta_range Cosine of the max extent of angle.
Vector3f cone_uniform_sample(const Point2f &u, Float cos_theta_range);
/// @brief Uniform sample a cone of directions around z basis.
/// @param cos_theta_range Cosine of the max extent of angle.
Vector3f cone_uniform_sample(const Point2f &u, Float cos_theta_range,
                             const Vector3f &x, const Vector3f &y,
                             const Vector3f &z);
Float cone_uniform_pdf(Float cos_theta_range);
/// @brief Uniform sample a triangle area.
/// @return Barycentric coord.
Point2f triangle_uniform_sample(const Point2f &u);

void fill_stratified_1D(Float *array, int n_samples, RNG &rng, bool jitter);
void fill_stratified_2D(Point2f *array, int n_x, int n_y, RNG &rng,
                        bool jitter);

/// @brief Shuffer array with multiple dimensions.
///        Layout should be dimension first.
template <typename T>
void shuffle(T *array, int n_samples, int n_dims, RNG &rng) {
  for (int i = 0; i < n_samples; ++i) {
    int other = i + rng.uniform_uint32(n_samples - i);
    for (int j = 0; j < n_dims; ++j) {
      std::swap(array[n_dims * i + j], array[n_dims * other + j]);
    }
  }
}

/// @brief Represents PDF and CDF to sample a piecewise-constant 1D function.
struct Distribution1D {
  /// @param values Step values defined over [0, 1].
  /// @param n_values Number of step values.
  Distribution1D(const Float *values, int n_values);
  /// @brief Sample this function as a continuous distribution.
  /// @param u Uniform random sample.
  /// @param pdf_value Store the result PDF value.
  /// @param offset Store the largest index
  ///               by which CDF[index] <= u < CDF[index + 1].
  /// @return A sample value with the distribution of this function.
  Float sample_continuous(Float u, Float *pdf_value,
                          int *offset = nullptr) const;
  /// @brief Discrete probability sample in 1, 2, ..., n with this distribution.
  /// @param u Uniform random sample.
  /// @param pdf_value Store the result PDF value.
  /// @param relative Store the relative distance in cdf range.
  /// @return The sampled value in i, 2, ..., n.
  int sample_discrete(Float u, Float *pdf_value,
                      Float *relative = nullptr) const;
  Float discrete_pdf(int index) const;
  /// @brief Get number of pieces.
  int count() const;

  std::vector<Float> func, cdf;
  Float integral;
};

/**
 *  target intergrand: f(x)g(x)dx
 * MIS estimator:
 *  1/nf sum( f(x)g(x)wf(x) / pf(x) ) + 1/ng sum( f(y)g(y)wg(y) / pg(y) )
 *  x, y: Partical samples.
 *  n[fg]: Number of samples taken on that distribution.
 *  w[fg]: Weighting function.
 *  p[fg]: pdf function.
 */

/// @brief Balance heuristic weighting function used for MIS method.
/// @param nf Number of samples taken on distribution f.
/// @param pdff pdf value for this sample in distribution f.
/// @param ng Number of samples taken on distribution g.
/// @param pdfg pdf value for this sample in distribution g.
inline Float balance_heuristic(int nf, Float pdff, int ng, Float pdfg) {
  return (nf * pdff) / (nf * pdff + ng * pdfg);
}
/// @brief Power (of 2) heuristic weighting function used for MIS method.
/// @param nf Number of samples taken on distribution f.
/// @param pdff pdf value for this sample in distribution f.
/// @param ng Number of samples taken on distribution g.
/// @param pdfg pdf value for this sample in distribution g.
inline Float power_heuristic(int nf, Float pdff, int ng, Float pdfg) {
  Float f = nf * pdff, g = ng * pdfg;
  return (f * f) / (f * f + g * g);
}
}  // namespace TRay