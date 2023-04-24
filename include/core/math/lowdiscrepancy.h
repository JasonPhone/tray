#pragma once
#include "core/TRay.h"
#include "core/math/RNG.h"

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
} // namespace TRay