#pragma once
#include "core/TRay.h"
#include "core/Texture.h"

namespace TRay {
template <typename T>
class ConstantTexture : public Texture<T> {
 public:
  ConstantTexture(const T &value): m_value(value) {}
  T evaluate(const SurfaceInteraction &si) const override { return m_value; }

 private:
  const T m_value;
};
}  // namespace TRay