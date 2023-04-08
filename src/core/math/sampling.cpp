#include "core/math/sampling.h"
#include "core/math/RNG.h"
namespace TRay {
Point2f disk_rejection_sample(RNG &rng) {
  Point2f p;
  do {
    p.x = 1 - 2 * rng.uniform_float();
    p.y = 1 - 2 * rng.uniform_float();
  } while (p.x * p.x + p.y * p.y > 1);
  return p;
}
Point2f disk_uniform_sample(const Point2f &u) {
  Float r = std::sqrt(u[0]);
  Float theta = 2 * PI * u[1];
  return Point2f(r * std::cos(theta), r * std::sin(theta));
}
Point2f disk_concentric_sample(const Point2f &u) {
  // Map uniform random numbers to [-1,1]^2.
  Point2f u_offset = 2.0 * u - Vector2f(1, 1);
  // Handle degeneracy at the origin.
  if (u_offset[0] == 0 && u_offset[1] == 0) return Point2f(0, 0);
  // Concentric map to point.
  Float theta, r;
  if (std::abs(u_offset[0]) > std::abs(u_offset[1])) {
    r = u_offset[0];
    theta = PI_DIV_4 * (u_offset[1] / u_offset[0]);
  } else {
    r = u_offset[1];
    theta = PI_DIV_2 - PI_DIV_4 * (u_offset[0] / u_offset[1]);
  }
  return r * Point2f(std::cos(theta), std::sin(theta));
}
Vector3f hemisphere_uniform_sample(const Point2f &u) {
  Float z = u[0];
  Float r = std::sqrt(std::max((Float)0, (Float)1. - z * z));
  Float phi = 2 * PI * u[1];
  return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}
Vector3f hemisphere_cosine_sample(const Point2f &u) {
  Point2f d = disk_concentric_sample(u);
  Float z = std::sqrt(std::max((Float)0, 1 - d.x * d.x - d.y * d.y));
  return Vector3f(d.x, d.y, z);
}
Vector3f sphere_uniform_sample(const Point2f &u) {
  Float z = 1 - 2 * u.x;
  Float r = std::sqrt(std::max((Float)0, (Float)1 - z * z));
  Float phi = 2 * PI * u.y;
  return Vector3f(r * std::cos(phi), r * std::sin(phi), z);
}
Vector3f cone_uniform_sample(const Point2f &u, Float cos_theta_range) {
  Float cos_theta = lerp(cos_theta_range, 1.f, u[0]);
  Float sin_theta = std::sqrt((Float)1 - cos_theta * cos_theta);
  Float phi = u[1] * 2 * PI;
  return Vector3f(std::cos(phi) * sin_theta, std::sin(phi) * sin_theta,
                  cos_theta);
}
Vector3f cone_uniform_sample(const Point2f &u, Float cos_theta_range,
                             const Vector3f &x, const Vector3f &y,
                             const Vector3f &z) {
  Vector3f dir = cone_uniform_sample(u, cos_theta_range);
  return dir[0] * x + dir[1] * y + dir[2] * z;
}
Point2f triangle_uniform_sample(const Point2f &u) {
 Float su0 = std::sqrt(u[0]);
 return Point2f(1 - su0, u[1] * su0);
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

Distribution1D::Distribution1D(const Float *values, int n_values)
    : func(values, values + n_values), cdf(n_values + 1) {
  // Size of cdf is one more for the final value of 1.
  // Evaluate the integral.
  cdf[0] = 0;
  for (int i = 0; i < n_values + 1; i++) {
    // The domain is splitted into n pieces. OK for non-equal cases.
    cdf[i] = cdf[i - 1] + func[i - 1] / n_values;
  }
  integral = cdf[n_values];
  // Normalize into CDF.
  if (integral == 0) {
    // 0 or negative values in func, use trivial CDF.
    for (int i = 1; i < n_values + 1; i++) {
      cdf[i] = Float(i) / Float(n_values);
    }
  } else {
    // We can use the integral to normalize all steps.
    for (int i = 1; i < n_values + 1; i++) {
      cdf[i] /= integral;
    }
  }
}
Float Distribution1D::sample_continuous(Float xi, Float *pdf,
                                        int *offset = nullptr) const {
  // Inversion method.
  int off =
      binary_search(cdf.size(), [&](int index) { return cdf[index] <= xi; });
  // cdf[0] is always 0, else ths off should goes back one.
  if (off) off--;
  if (offset) *offset = off;
  Float delta = xi - cdf[off];
  ASSERT(delta >= -TRAY_EPS);
  // Normalize in the range.
  if (cdf[off] < cdf[off + 1]) delta /= (cdf[off + 1] - cdf[off]);
  if (pdf) *pdf = func[off] / integral;
  // Mapped back to x axis.
  return (off + delta) / count();
}
int Distribution1D::sample_discrete(Float xi, Float *pdf,
                                    Float *relative) const {
  int off =
      binary_search(cdf.size(), [&](int index) { return cdf[index] <= xi; });
  // cdf[0] is always 0, else ths off should goes back one.
  if (off) off--;
  if (pdf) *pdf = discrete_pdf(off);
  ASSERT(xi - cdf[off] >= -TRAY_EPS);
  if (relative) *relative = (xi - cdf[off]) / (cdf[off + 1] - cdf[off]);
  return off;
}
Float Distribution1D::discrete_pdf(int index) const {
  ASSERT(0 <= index && index < count());
  return func[index] / integral / count();
}
int Distribution1D::count() const { return func.size(); }
}  // namespace TRay