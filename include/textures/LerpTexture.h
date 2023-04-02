#pragma once
#include "core/TRay.h"
#include "core/Texture.h"

namespace TRay {
/// @brief Use a Float texture to lerp between other two textures.
template <typename T>
class LerpTexture : public Texture<T> {
 public:
  /// @param texture1 Lerp a.
  /// @param texture2 Lerp b.
  /// @param lerpt Lerp t.
  LerpTexture(const std::shared_ptr<Texture<T>> &texture1,
              const std::shared_ptr<Texture<T>> &texture2,
              const std::shared_ptr<Texture<Float>> &lerpt)
      : m_tex1(texture1), m_tex2(texture2), m_lerpt(lerpt) {}
  T evaluate(const SurfaceInteraction &si) const override {
    T a = m_tex1->evaluate(si);
    T b = m_tex2->evaluate(si);
    Float t = m_lerpt->evaluate(si);
    return (1.0 - t) * a + t * b;
  }

 private:
  std::shared_ptr<Texture<T>> m_tex1, m_tex2;
  std::shared_ptr<Texture<Float>> m_lerpt;
};
}  // namespace TRay