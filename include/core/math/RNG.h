#pragma once
#include "core/TRay.h"
// #include <random>
/**
 * @brief Random Number Generator used for all stochastic of TRay.
 *
 * See https://stackoverflow.com/questions/7114043
 * if you want std::mt19937 from <random>.
 */
namespace TRay {
// Random Number Declarations
static const double DOUBLE_ONE_M_EPS = 0x1.fffffffffffffp-1;
static const float FLOAT_ONE_M_EPS = 0x1.fffffep-1;

#ifdef TRAY_FLOAT_AS_DOUBLE
static const Float ONE_M_EPS = DOUBLE_ONE_M_EPS;
#else
static const Float ONE_M_EPS = FLOAT_ONE_M_EPS;
#endif

#define PCG32_DEFAULT_STATE 0x853c49e6748fea9bULL
#define PCG32_DEFAULT_STREAM 0xda3e39cb94b95bdbULL
#define PCG32_MULT 0x5851f42d4c957f2dULL
class RNG {
 public:
  RNG();
  RNG(uint64_t sequenceIndex);
  void set_sequence(uint64_t init_seq);
  /// @brief Uniform distribution in [0, 2^32 - 1].
  uint32_t uniform_uint32();
  /// @brief Uniform distribution in [0, b-1].
  uint32_t uniform_uint32(uint32_t b);
  /// @brief Uniform Float distribution in [0, 1).
  Float uniform_float() {
    return std::min(ONE_M_EPS, Float(uniform_uint32() * 0x1p-32f));
  }
  template <typename ITR>
  void shuffle(ITR begin, ITR end) {
    for (ITR it = end - 1; it > begin; --it)
      std::iter_swap(it, begin + uniform_uint32((uint32_t)(it - begin + 1)));
  }

 private:
  uint64_t m_state, m_inc;
  // std::mt19937 m_generator;
};

inline RNG::RNG(): m_state(PCG32_DEFAULT_STATE), m_inc(PCG32_DEFAULT_STREAM) {}
inline RNG::RNG(uint64_t idx_init_seq) { set_sequence(idx_init_seq); }
inline void RNG::set_sequence(uint64_t idx_init_seq) {
  // m_generator.seed(idx_init_seq);
  // return;
  m_state = 0u;
  m_inc = (idx_init_seq << 1u) | 1u;
  uniform_uint32();
  m_state += PCG32_DEFAULT_STATE;
  uniform_uint32();
}

inline uint32_t RNG::uniform_uint32() {
  // return m_generator();
  // PCG minimal impl.
  uint64_t oldstate = m_state;
  m_state = oldstate * PCG32_MULT + m_inc;
  uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
  uint32_t rot = (uint32_t)(oldstate >> 59u);
  return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31));
}
inline uint32_t RNG::uniform_uint32(uint32_t b) {
  // Ensure the uniform.
  uint32_t threshold = (~b + 1u) % b;
  // Reject.
  while (true) {
    uint32_t r = uniform_uint32();
    if (r >= threshold) return r % b;
  }
}
}  // namespace TRay