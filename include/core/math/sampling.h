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
Float disk_uniform_pdf() { return PI_INV; }
Vector3f hemisphere_uniform_sample(const Point2f &u);
Float hemisphere_uniform_pdf() { return PI_INV2; }
/// @brief Sample a point on (0, 0, 1) hemishpere with cos(theta) distribution.
Vector3f hemisphere_cosine_sample(const Point2f &u);
Float hemisphere_cosine_pdf(Float cos_theta) { return cos_theta * PI_INV; }
Vector3f sphere_uniform_sample(const Point2f &u);
Float sphere_uniform_pdf() { return PI_INV4; }
/// @brief Uniform sample a cone of directions around (0, 0, 1).
/// @param cos_theta_range Cosine of the max extent of angle.
Vector3f cone_uniform_sample(const Point2f &u, Float cos_theta_range);
/// @brief Uniform sample a cone of directions around z basis.
/// @param cos_theta_range Cosine of the max extent of angle.
Vector3f cone_uniform_sample(const Point2f &u, Float cos_theta_range,
                             const Vector3f &x, const Vector3f &y,
                             const Vector3f &z);
Float cone_uniform_pdf(Float cos_theta_range) {
  return 1 / (2 * PI * (1 - cos_theta_range));
}
/// @brief Uniform sample a triangle area.
/// @return Barycentric coord.
Point2f triangle_uniform_sample(const Point2f &u);

void fill_stratified_1D(Float *array, int n_samples, RNG &rng, bool jitter);
void fill_stratified_2D(Point2f *array, int n_x, int n_y, RNG &rng,
                        bool jitter);

/// @brief Shuffer array with multiple dimensions.
///        Layout should be dimension first.
template <typename T>
void shuffle(T *array, int count, int n_dim, RNG &rng) {
  for (int i = 0; i < count; ++i) {
    int other = i + rng.uniform_uint32(count - i);
    for (int j = 0; j < n_dim; ++j)
      std::swap(array[n_dim * i + j], array[n_dim * other + j]);
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
  Float sample_continuous(Float u, Float *pdf_value, int *offset = nullptr) const;
  /// @brief Discrete probability sample in 1, 2, ..., n with this distribution.
  /// @param u Uniform random sample.
  /// @param pdf_value Store the result PDF value.
  /// @param relative Store the relative distance in cdf range.
  /// @return The sampled value in i, 2, ..., n.
  int sample_discrete(Float u, Float *pdf_value, Float *relative) const;
  Float discrete_pdf(int index) const;
  /// @brief Get number of pieces.
  int count() const;

  std::vector<Float> func, cdf;
  Float integral;
};
}  // namespace TRay