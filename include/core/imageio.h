#pragma once
#include "core/TRay.h"

namespace TRay {
void image_to_array(Float *src, uint8_t *dst, int width, int height) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
// Gamma correction and scale to [0, 255].
#define uint8RGB(v) (uint8_t) clamp(255.0 * gamma_correct(v) + 0.5, 0.0, 255.0)
      dst[0] = uint8RGB(src[(y * width + x) * 3 + 0]);
      dst[1] = uint8RGB(src[(y * width + x) * 3 + 1]);
      dst[2] = uint8RGB(src[(y * width + x) * 3 + 2]);
#undef uint8RGB(v)
      dst += 3;
    }
  }
}
}  // namespace TRay