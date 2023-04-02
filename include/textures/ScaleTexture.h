#pragma once
#include "core/TRay.h"
#include "core/Texture.h"

namespace TRay {
template <typename T1, typename T2>
class ScaleTexture : public Texture<T2> {
 public:
  ScaleTexture(const std::shared_ptr<Texture<T1>> &texture1,
               const std::shared_ptr<Texture<T2>> &texture2)
      : m_tex1(texture1), m_tex2(texture2) {}
  T2 evaluate(const SurfaceInteraction &si) const override {
    return m_tex1->evaluate(si) * m_tex2->evaluate(si);
  }

 private:
  std::shared_ptr<Texture<T1>> m_tex1;
  std::shared_ptr<Texture<T2>> m_tex2;
};
}  // namespace TRay