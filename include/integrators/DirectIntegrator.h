#pragma once
#include "core/Integrator.h"
#include "core/TRay.h"

/**
 * DirectIntegrator accounts for the light from light source directly to the
 * shading point. The specular reflection and transmission is also included.
 * Preprocess: Request sample arrays from sampler if splitting is used.
 * Shading:
 *   Find intersection.
 *   Local shading.
 *     Fill BSDF.
 *     Self emissive.
 *     **Direct lighting**
 *       MIS is used.
 *   Tracing for specular reflection and transmission.
 */

/// @brief How to sample the light sources. Both unbaised.
enum class LightSample {
  // Use splitting to uniformly sample all light sources,
  // Light::m_n_samples (may be rounded by sampler) samples for each source.
  UNIFORM_ALL,
  // Randomly choose one light source and take one sample.
  // Suitable for high spp.
  UNIFORM_ONE
};
namespace TRay {
class DirectIntegrator : public SamplerIntegrator {
 public:
  DirectIntegrator(const LightSample &light_sample_method, int max_depth,
                   std::shared_ptr<const Camera> camera,
                   std::shared_ptr<Sampler> sampler)
      : SamplerIntegrator(camera, sampler),
        m_light_sample(light_sample_method),
        m_max_depth(max_depth) {}
  void preprocess(const Scene &scene, Sampler &sampler) override;
  Spectrum Li(const Ray &ray, const Scene &scene, Sampler &sampler,
              int depth = 0) const override;

 private:
  const LightSample m_light_sample;
  /// @brief Max recursion depth for specular reflection and transmission.
  const int m_max_depth;
  std::vector<int> m_n_light_samples;
};
}  // namespace TRay