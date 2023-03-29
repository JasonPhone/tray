#include "core/reflection/Lambertian.h"

namespace TRay {
Spectrum LambertianReflection::f(const Vector3f &, const Vector3f &) const {
  return m_scale * PI_INV;
}
Spectrum LambertianTransmission::f(const Vector3f &, const Vector3f &) const {
  return m_scale * PI_INV;
}
}  // namespace TRay