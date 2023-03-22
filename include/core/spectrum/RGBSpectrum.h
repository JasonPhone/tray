#pragma once
#include "core/spectrum/CoefficientSpectrum.h"

namespace TRay {
class RGBSpectrum : public CoefficientSpectrum<3> {
 public:
  /// @brief Build an RGBSpectrum from Float array and given SpectrumType
  static RGBSpectrum from_RGB(const Float rgb[3],
                              SpectrumType type = SpectrumType::Reflectance) {
    RGBSpectrum s;
    s.c[0] = rgb[0];
    s.c[1] = rgb[1];
    s.c[2] = rgb[2];
    return s;
  }
  // TODO By now we only work on RGB values, but the real one should be upon
  // sampled spectrums. static RGBSpectrum from_sampled(const Float *lambdas,
  // const Float *values, int n) {}
  RGBSpectrum(Float init_v = 0) : CoefficientSpectrum<3>(init_v) {}
  RGBSpectrum(const CoefficientSpectrum<3> &cs) : CoefficientSpectrum<3>(cs) {}
  /// @brief Convert to a plain RGB array.
  /// @param rgb Pointer to array, at least 3 in size.
  void to_RGB(Float *rgb) const {
    rgb[0] = c[0];
    rgb[1] = c[1];
    rgb[2] = c[2];
  }
  const RGBSpectrum &to_RGBSpectrum() const { return *this; }
};
}  // namespace TRay