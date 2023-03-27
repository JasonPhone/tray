#pragma once
#include "core/TRay.h"
#include "core/geometry/Point.h"
#include "core/math/RNG.h"

namespace TRay {
/// @brief Mapping a [0, 1)^2 point to 2D disk by concentric mapping.
/// @param u
/// @return
Point2f square_to_disk_concentric(const Point2f &u);
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
}  // namespace TRay