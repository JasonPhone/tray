#pragma once
#include "core/TRay.h"
#include "core/math/sampling.h"
#include "core/geometry/Point.h"
#include "core/math/SobolMatrices.h"

namespace TRay {
/// @brief Get the inversed value of @param a based on @param base_idx-th prime.
Float radical_inverse(int base_idx, uint64_t a);
/// @brief Restore the inverse_digits in function radical_inverse().
/// @param inverse_digits
/// @param n_digits Length of original value, for restoring zeros.
/// @return
template <int base_val>
inline uint64_t radical_inverse_inv(uint64_t inverse_digits, int n_digits) {
  uint64_t index = 0;
  for (int i = 0; i < n_digits; ++i) {
    uint64_t digit = inverse_digits % base_val;
    inverse_digits /= base_val;
    index = index * base_val + digit;
  }
  return index;
}
/// @brief Perform a multiply of Ca.
/// @param C The first column of 32x32 generator matrix.
/// @param a The number to be "radical-inversed".
inline uint32_t apply_generator(const uint32_t *C, uint32_t a) {
  uint32_t ret = 0;
  // The least significant bit of a selects the first column.
  for (int i = 0; a; i++, a >>= 1)
    if (a & 1) ret ^= C[i];
  return ret;
}
/// @brief Get the sample value of generator mat C and value a.
/// @param C The first column of 32x32 generator matrix.
/// @param a The number to be "radical-inversed".
/// @param scramble_bits Encoded scramble bits.
inline Float sample_generator_mat(const uint32_t *C, uint32_t a,
                                  uint32_t scramble_bits = 0) {
  return (apply_generator(C, a) ^ scramble_bits) * 0x1p-32f;
}
// /// @brief Get the @param n-th Gray code. Starting from 0.
// inline uint32_t gray_code(uint32_t n) { return (n >> 1) ^ n; }

/// @brief Fill a series of sample values by generator matrix.
/// @param C The first column of 32x32 generator matrix.
/// @param n Number of sample values to be filled.
/// @param scramble_bits Encoded scramble bits.
/// @param p Store the sample values.
inline void fill_sample_by_graycode(const uint32_t *C, uint32_t n,
                                    uint32_t scramble_bits, Float *p) {
  uint32_t v = scramble_bits;  // Fisrt value.
  for (uint32_t i = 0; i < n; ++i) {
    p[i] = v * 0x1p-32f;
    // Next value.
    // The changed bit position of Gray code is equal to
    // # of trailing zeros.
    // i.e. from 011-th to 100-th code, which is 010 to 110,
    // the changed bit index is 2, # trailing zeros of 100.
    v ^= C[ctz(i + 1)];
  }
}
/// @brief 2D version.
inline void fill_sample_by_graycode(const uint32_t *C0, const uint32_t *C1,
                                    uint32_t n, const Point2i &scramble_bits,
                                    Point2f *p) {
  uint32_t v[2] = {(uint32_t)scramble_bits.x, (uint32_t)scramble_bits.y};
  for (uint32_t i = 0; i < n; ++i) {
    p[i].x = std::min(v[0] * Float(0x1p-32), ONE_M_EPS);
    p[i].y = std::min(v[1] * Float(0x1p-32), ONE_M_EPS);
    v[0] ^= C0[ctz(i + 1)];
    v[1] ^= C1[ctz(i + 1)];
  }
}
inline void fill_VDCorput_1D(int n_sub_samples, int n_pxl_samples,
                             Float *sample_values, RNG &rng) {
  uint32_t scramble_bits = rng.uniform_uint32();
  // Generator matrix. Identity.
  const uint32_t CVanDerCorput[32] = {
      // clang-format off
      0x80000000, 0x40000000, 0x20000000, 0x10000000,
      0x8000000,  0x4000000,  0x2000000,  0x1000000,
      0x800000,   0x400000,   0x200000,   0x100000,
      0x80000,    0x40000,    0x20000,    0x10000,
      0x8000,     0x4000,     0x2000,     0x1000,
      0x800,      0x400,      0x200,      0x100,
      0x80,       0x40,       0x20,       0x10,
      0x8,        0x4,        0x2,        0x1
      // clang-format on
  };
  int n_total_samples = n_sub_samples * n_pxl_samples;
  fill_sample_by_graycode(CVanDerCorput, n_total_samples, scramble_bits,
                          sample_values);
  // Shuffle to cancel the correlation.
  for (int i = 0; i < n_pxl_samples; ++i)
    shuffle(sample_values + i * n_sub_samples, n_sub_samples, 1, rng);
  shuffle(sample_values, n_pxl_samples, n_sub_samples, rng);
}
inline void fill_Sobol_2D(int n_sub_samples, int n_pxl_samples,
                          Point2f *sample_values, RNG &rng) {
  Point2i scramble;
  scramble[0] = rng.uniform_uint32();
  scramble[1] = rng.uniform_uint32();
  // Two matrices.
  const uint32_t CSobol[2][32] = {
      // clang-format off
      {
        0x80000000, 0x40000000, 0x20000000, 0x10000000, 
        0x8000000,  0x4000000,  0x2000000,  0x1000000,
        0x800000,   0x400000,   0x200000,   0x100000,
        0x80000,    0x40000,    0x20000,    0x10000,
        0x8000,     0x4000,     0x2000,     0x1000,
        0x800,      0x400,      0x200,      0x100,
        0x80,       0x40,       0x20,       0x10,
        0x8,        0x4,        0x2,        0x1
      },
      {
        0x80000000, 0xc0000000, 0xa0000000, 0xf0000000,
        0x88000000, 0xcc000000, 0xaa000000, 0xff000000, 
        0x80800000, 0xc0c00000, 0xa0a00000, 0xf0f00000,
        0x88880000, 0xcccc0000, 0xaaaa0000, 0xffff0000, 
        0x80008000, 0xc000c000, 0xa000a000, 0xf000f000,
        0x88008800, 0xcc00cc00, 0xaa00aa00, 0xff00ff00,
        0x80808080, 0xc0c0c0c0, 0xa0a0a0a0, 0xf0f0f0f0, 
        0x88888888, 0xcccccccc, 0xaaaaaaaa, 0xffffffff
      }
      // clang-format on
  };
  fill_sample_by_graycode(CSobol[0], CSobol[1], n_sub_samples * n_pxl_samples,
                          scramble, sample_values);
  // Shuffle to cancel the correlation.
  for (int i = 0; i < n_pxl_samples; ++i)
    shuffle(sample_values + i * n_sub_samples, n_sub_samples, 1, rng);
  shuffle(sample_values, n_pxl_samples, n_sub_samples, rng);
}
extern uint32_t CMaxMinDis[17][32];

// TODO how does this work?
inline uint64_t SobolIntervalToIndex(const uint32_t expo, uint64_t local_idx,
                                     const Point2i &pxl) {
  if (expo == 0) return 0;

  const uint32_t m2 = expo << 1;
  uint64_t index = uint64_t(local_idx) << m2;

  uint64_t delta = 0;
  for (int c = 0; local_idx; local_idx >>= 1, ++c)
    if (local_idx & 1)  // Add flipped column m + c + 1.
      delta ^= VDCSobolMatrices[expo - 1][c];

  // flipped b
  uint64_t b = ((uint64_t(uint32_t(pxl.x)) << expo) | uint32_t(pxl.y)) ^ delta;

  for (int c = 0; b; b >>= 1, ++c)
    if (b & 1)  // Add column 2 * m - c.
      index ^= VDCSobolMatricesInv[expo - 1][c];

  return index;
}

inline Float sample_Sobol(int64_t global_index, int dimension,
                          uint64_t scramble = 0) {
#ifdef TRAY_FLOAT_AS_DOUBLE
  return sample_Sobol_double(global_index, dimension, scramble);
#else
  return sample_Sobol_float(global_index, dimension, scramble);
#endif
}
inline float sample_Sobol_float(int64_t a, int dimension, uint32_t scramble) {
  if (dimension >= NSobolDimensions) {
    SWarn("sample_Sobol_float: dimension index exceeded, zero is returned.");
    return 0;
  }
  uint32_t v = scramble;
  for (int i = dimension * SobolMatrixSize; a != 0; a >>= 1, ++i)
    if (a & 1) v ^= SobolMatrices32[i];
  return v * 0x1p-32f;
}
inline float sample_Sobol_double(int64_t a, int dimension, uint32_t scramble) {
  if (dimension >= NSobolDimensions) {
    SWarn("sample_Sobol_float: dimension index exceeded, zero is returned.");
    return 0;
  }
  uint64_t result = scramble & ~-(1ll << SobolMatrixSize);
  for (int i = dimension * SobolMatrixSize; a != 0; a >>= 1, i++)
    if (a & 1) result ^= SobolMatrices64[i];
  return std::min(result * (1.0 / (1ull << SobolMatrixSize)), ONE_M_EPS);
}
}  // namespace TRay