#pragma once
#include "core/TRay.h"
#include "core/Texture.h"

namespace TRay {
/// @brief Use a mapped (s, t) pair to bilerp between 4 constants.
template <typename T>
class BiLerpTexture : public Texture<T> {
 public:
  BiLerpTexture(std::shared_ptr<TextureMapping2D> &mapping, const T v00,
                const T v01, const T v10, const T v11)
      : m_mapping(mapping), m_v00(v00), m_v01(v01), m_v10(v10), m_v11(v11) {}
  T evaluate(const SurfaceInteraction &si) const override {
    Point2f st = m_mapping->map(si);
    // clang-format off
    return (1.0 - st.x) * (1.0 - st.y) * m_v00 + (1.0 - st.x) * st.y * m_v01 +
           (      st.x) * (1.0 - st.y) * m_v10 + (      st.x) * st.y * m_v11;
    // clang-format on
  }

 private:
  std::shared_ptr<TextureMapping2D> m_mapping;
  const T m_v00, m_v01, m_v10, m_v11;
};
}  // namespace TRay