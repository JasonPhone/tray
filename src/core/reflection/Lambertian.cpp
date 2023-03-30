#include "core/reflection/Lambertian.h"

namespace TRay {
Spectrum LambertianReflection::f(const Vector3f &, const Vector3f &) const {
  return m_scale * PI_INV;
}
std::string LambertianReflection::to_string() const {
  return "[ LambertianReflection scale: " + m_scale.to_string() + " ]";
}

Spectrum LambertianTransmission::f(const Vector3f &, const Vector3f &) const {
  return m_scale * PI_INV;
}
std::string LambertianTransmission::to_string() const {
  return "[ LambertianTransmission scale: " + m_scale.to_string() + " ]";
}
}  // namespace TRay