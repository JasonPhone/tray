#pragma once
#include "core/TRay.h"

namespace TRay {
enum class SpectrumType { Reflectance, Illuminant };
template <int n_spec_samples>
class CoefficientSpectrum {
 public:
  CoefficientSpectrum(Float init_v = 0.0) {
    std::fill(c, c + n_spec_samples, init_v);
    ASSERT(!has_NaN());
  }
  bool has_NaN() const {
    for (int i = 0; i < n_spec_samples; i++)
      if (std::isnan(c[i])) return true;
    return false;
  }
  bool is_black() const {
    for (int i = 0; i < n_spec_samples; i++)
      if (c[i] != 0.0) return false;
    return true;
  }
  CoefficientSpectrum &operator+=(const CoefficientSpectrum &other) {
    ASSERT(!other.has_NaN());
    for (int i = 0; i < n_spec_samples; i++) c[i] += other.c[i];
    ASSERT(!has_NaN());
    return *this;
  }
  CoefficientSpectrum operator+(const CoefficientSpectrum &other) const {
    ASSERT(!other.has_NaN());
    CoefficientSpectrum ret = *this;
    for (int i = 0; i < n_spec_samples; i++) ret.c[i] += other.c[i];
    ASSERT(!ret.has_NaN());
    return ret;
  }
  CoefficientSpectrum &operator-=(const CoefficientSpectrum &other) {
    ASSERT(!other.has_NaN());
    for (int i = 0; i < n_spec_samples; i++) c[i] -= other.c[i];
    ASSERT(!has_NaN());
    return *this;
  }
  CoefficientSpectrum operator-(const CoefficientSpectrum &other) const {
    ASSERT(!other.has_NaN());
    CoefficientSpectrum ret = *this;
    for (int i = 0; i < n_spec_samples; i++) ret.c[i] -= other.c[i];
    ASSERT(!ret.has_NaN());
    return ret;
  }
  CoefficientSpectrum operator-() const {
    CoefficientSpectrum ret = *this;
    for (int i = 0; i < n_spec_samples; i++) ret.c[i] = -ret.c[i];
    ASSERT(!ret.has_NaN());
    return ret;
  }
  CoefficientSpectrum operator*(const CoefficientSpectrum &other) const {
    ASSERT(!other.has_NaN());
    CoefficientSpectrum ret = *this;
    for (int i = 0; i < n_spec_samples; i++) ret.c[i] *= other.c[i];
    ASSERT(!ret.has_NaN());
    return ret;
  }
  CoefficientSpectrum &operator*=(const CoefficientSpectrum &other) {
    ASSERT(!other.has_NaN());
    for (int i = 0; i < n_spec_samples; i++) c[i] *= other.c[i];
    ASSERT(!has_NaN());
    return *this;
  }
  CoefficientSpectrum operator*(Float s) const {
    CoefficientSpectrum ret = *this;
    for (int i = 0; i < n_spec_samples; i++) ret.c[i] *= s;
    ASSERT(!ret.has_NaN());
    return ret;
  }
  CoefficientSpectrum &operator*=(Float s) {
    for (int i = 0; i < n_spec_samples; i++) c[i] *= s;
    ASSERT(!has_NaN());
    return *this;
  }
  friend inline CoefficientSpectrum operator*(Float s,
                                              const CoefficientSpectrum &cs) {
    return cs * s;
  }
  CoefficientSpectrum operator/(const CoefficientSpectrum &other) const {
    ASSERT(!other.has_NaN());
    CoefficientSpectrum ret = *this;
    for (int i = 0; i < n_spec_samples; i++) {
      ASSERT(other.c[i] != 0);
      ret.c[i] /= other.c[i];
    }
    ASSERT(!ret.has_NaN());
    return ret;
  }
  CoefficientSpectrum operator/(Float s) const {
    ASSERT(s != 0.0);
    CoefficientSpectrum ret = *this;
    for (int i = 0; i < n_spec_samples; i++) {
      ret.c[i] /= s;
    }
    ASSERT(!ret.has_NaN());
    return ret;
  }
  CoefficientSpectrum &operator/=(Float s) const {
    ASSERT(s != 0.0);
    for (int i = 0; i < n_spec_samples; i++) c[i] /= s;
    ASSERT(!has_NaN());
    return *this;
  }
  bool operator==(const CoefficientSpectrum &other) const {
    for (int i = 0; i < n_spec_samples; i++)
      if (c[i] != other.c[i]) return false;
  }
  bool operator!=(const CoefficientSpectrum &other) const {
    return !(*this == other);
  }
  /// @note This implies the SPD is linearly scaling a set of basis functions.
  Float &operator[](int i) { return c[i]; }

  /// @brief Used by reflection models.

  friend CoefficientSpectrum sqrt(const CoefficientSpectrum &s) {
    CoefficientSpectrum ret;
    for (int i = 0; i < n_spec_samples; i++) ret.c[i] = std::sqrt(s.c[i]);
    ASSERT(!ret.has_NaN());
    return ret;
  }
  friend CoefficientSpectrum pow(const CoefficientSpectrum &s, Float e) {
    CoefficientSpectrum ret;
    for (int i = 0; i < n_spec_samples; ++i) ret.c[i] = std::pow(s.c[i], e);
    ASSERT(!ret.has_NaN());
    return ret;
  }
  friend CoefficientSpectrum exp(const CoefficientSpectrum &s) {
    CoefficientSpectrum ret;
    for (int i = 0; i < n_spec_samples; ++i) ret.c[i] = std::exp(s.c[i]);
    ASSERT(!ret.has_NaN());
    return ret;
  }

  friend std::ostream &operator<<(std::ostream &os,
                                  const CoefficientSpectrum &s) {
    return os << s.to_string();
  }
  std::string to_string() const {
    std::string str = "[";
    for (int i = 0; i < n_spec_samples; ++i) {
      // TODO Make this a func?
      size_t size =
          snprintf(nullptr, 0, "%.2f", c[i]) + 1;  // +1 for trailing \0
      std::string s;
      s.resize(size);
      snprintf(&s[0], size, "%.2f", c[i]);
      s.pop_back();  // remove trailing \0
      str += s;
      if (i + 1 < n_spec_samples) str += ", ";
    }
    str += "]";
    return str;
  }

  static const int n_samples = n_spec_samples;

 protected:
  Float c[n_spec_samples];
};
}  // namespace TRay