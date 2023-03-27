#include "core/math/sampling.h"
#include "core/math/RNG.h"
namespace TRay {
/// @brief Mapping a [0, 1)^2 point to 2D disk by concentric mapping.
/// @param u
/// @return
Point2f square_to_disk_concentric(const Point2f &u) {
  // Map uniform random numbers to [-1,1]^2.
  Point2f u_offset = 2.0 * u - Vector2f(1, 1);
  // Handle degeneracy at the origin.
  if (u_offset.x == 0 && u_offset.y == 0) return Point2f(0, 0);
  // Concentric map to point.
  Float theta, r;
  if (std::abs(u_offset.x) > std::abs(u_offset.y)) {
    r = u_offset.x;
    theta = PI_DIV_4 * (u_offset.y / u_offset.x);
  } else {
    r = u_offset.y;
    theta = PI_DIV_2 - PI_DIV_4 * (u_offset.x / u_offset.y);
  }
  return r * Point2f(std::cos(theta), std::sin(theta));
}
void fill_stratified_1D(Float *array, int n_samples, RNG &rng, bool jitter) {
  Float n_samples_inv = (Float)1 / n_samples;
  for (int i = 0; i < n_samples; ++i) {
    Float delta = jitter ? rng.uniform_float() : 0.5;
    array[i] = std::min((i + delta) * n_samples_inv, ONE_M_EPS);
  }
}
void fill_stratified_2D(Point2f *array, int n_x, int n_y, RNG &rng,
                        bool jitter) {
  Float dx = (Float)1 / n_x, dy = (Float)1 / n_y;
  for (int y = 0; y < n_y; ++y)
    for (int x = 0; x < n_x; ++x) {
      Float jx = jitter ? rng.uniform_float() : 0.5f;
      Float jy = jitter ? rng.uniform_float() : 0.5f;
      array->x = std::min((x + jx) * dx, ONE_M_EPS);
      array->y = std::min((y + jy) * dy, ONE_M_EPS);
      ++array;
    }
}
}  // namespace TRay