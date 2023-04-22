#pragma once
#include "core/TRay.h"
#include "core/Texture.h"
#include "core/spectrum/spectrum.h"
#include "core/geometry/Vector.h"

namespace TRay {
class Grid3DTexture : public Texture<Spectrum> {
 public:
  Grid3DTexture(const Spectrum& color_a, const Spectrum& color_b,
                const Vector3f& interval)
      : m_color_a(color_a), m_color_b(color_b), m_interval(interval) {}
  Spectrum evaluate(const SurfaceInteraction& si) const override {
    int x = std::floor(si.p.x / m_interval.x);
    int y = std::floor(si.p.y / m_interval.y);
    int z = std::floor(si.p.z / m_interval.z);
    return (x + y + z) % 2 ? m_color_a : m_color_b;
  }

 private:
  const Spectrum m_color_a, m_color_b;
  const Vector3f m_interval;
};
}  // namespace TRay