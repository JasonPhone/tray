#pragma once
#include "core/TRay.h"
#include "core/reflection/BxDF.h"

namespace TRay {
class LambertianReflection : public BxDF {
 public:
  LambertianReflection(const Spectrum &scale)
      : BxDF(BxDFType(BSDF_DIFFUSE | BSDF_REFLECTION)), m_scale(scale) {}
  virtual Spectrum f(const Vector3f &wo, const Vector3f &wi) const override;
  Spectrum rho(const Vector3f &, int, const Point2f *) const override {
    return m_scale;
  }
  Spectrum rho(int, const Point2f *, const Point2f *) const override {
    return m_scale;
  }
  std::string to_string() const override;

 private:
  const Spectrum m_scale;
};

class LambertianTransmission : public BxDF {
 public:
  // LambertianTransmission Public Methods
  LambertianTransmission(const Spectrum &scale)
      : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_DIFFUSE)), m_scale(scale) {}
  Spectrum f(const Vector3f &wo, const Vector3f &wi) const override;
  Spectrum rho(const Vector3f &, int, const Point2f *) const override {
    return m_scale;
  }
  Spectrum rho(int, const Point2f *, const Point2f *) const override {
    return m_scale;
  }
  // TODO impl later.
  Spectrum sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
                    Float *pdf_value, BxDFType *sampledType) const override;
  Float pdf(const Vector3f &wo, const Vector3f &wi) const override;
  std::string to_string() const override;

 private:
  // LambertianTransmission Private Data
  Spectrum m_scale;
};
}  // namespace TRay