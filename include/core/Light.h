#pragma once
#include "core/TRay.h"
#include "core/geometry/Transform.h"
#include "core/spectrum/spectrum.h"
#include "core/geometry/Interaction.h"

namespace TRay {
enum LightType {
  LIGHT_DELTA_POSITION = 1 << 0,
  LIGHT_DELTA_DIRECTION = 1 << 1,
  LIGHT_AREA = 1 << 2,
  LIGHT_INFINITE = 1 << 3
};

class Light {
 public:
  /// @param type Type of light source.
  /// @param light_to_world Transformation.
  /// @param n_samples Number of samples been taken.
  Light(LightType type, const Transform &light_to_world, int n_samples = 1);
  /// @brief Compute the radiance arriving at the interaction time and
  ///        (reference) point from this light source. The incident direction
  ///        wi, pdf_value and visibility tester will also be initialized.
  /// @param ref Interaction.
  /// @param u Point on light source if MCM is used.
  /// @param wi Store the incident direction to the light source.
  /// @param pdf_value Store the pdf_value.
  /// @param vis Store the tester. Remain uninitialized if radiance is black.
  virtual Spectrum sample_Li(const Interaction &ref, const Point2f &u,
                             Vector3f *wi, Float *pdf_value,
                             VisibilityTester *vis) const = 0;
  virtual Float pdf_Li(const Interaction &ref, const Vector3f &wi) const = 0;
  /// @brief Get radiance as the "environment".
  Spectrum Le(const Ray &ray) const { return Spectrum(0.f); }
  /// @brief Get the total power this light source emits.
  /// @details This may be useful for adaptive light resources computation.
  virtual Spectrum total_power() const = 0;
  /// @brief Preprocess before rendering, e.g compute the scene bound.
  /// @param scene
  virtual void preprocess(const Scene &scene) {}
  /// @brief If the ligh is defined with delta distribution.
  bool is_delta_light() const {
    return (m_type & LIGHT_DELTA_POSITION) || (m_type & LIGHT_DELTA_DIRECTION);
  }

  // Type of light.
  const LightType m_type;
  // Number of samples taken for this light source.
  const int m_n_samples;

 protected:
  const Transform m_light_to_world;
};

/// @brief A closure, making light sources able to return radiance assuming that
///        the reference point and light source can see each other.
class VisibilityTester {
 public:
  VisibilityTester() {}
  VisibilityTester(const Interaction &ref0, const Interaction &ref1)
      : m_ref0(ref0), m_ref1(ref1) {}
  const Interaction &REF0() const { return m_ref0; }
  const Interaction &REF1() const { return m_ref1; }
  bool blocked(const Scene &scene) const;

 private:
  // Two ends of a shadow ray.
  Interaction m_ref0, m_ref1;
};

class AreaLight : public Light {
 public:
  AreaLight(const Transform &LightToWorld, int nSamples)
      : Light(LightType::LIGHT_AREA, LightToWorld, nSamples) {}
  /// @brief Compute the emitted radiance from given point and direction.
  virtual Spectrum L(const Interaction &inter, const Vector3f &w) const = 0;
};
}  // namespace TRay