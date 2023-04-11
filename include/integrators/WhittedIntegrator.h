#pragma once
#include "core/TRay.h"
#include "core/Integrator.h"
#include "core/stringformat.h"

namespace TRay {
/**
 * @brief Whitted integrator.
 * Features:
 *   Reflected and transmitted light from specular surface.
 *   Does not include other indirect light.
 *
 */
class WhittedIntegrator : public SamplerIntegrator {
 public:
  WhittedIntegrator(int max_depth, std::shared_ptr<const Camera> camera,
                    std::shared_ptr<Sampler> sampler)
      : SamplerIntegrator(camera, sampler), m_max_depth(max_depth) {
    SInfo(string_format("WhittedIntegrator:: Integrator with max depth %d. ",
                        max_depth));
  }
  Spectrum Li(const Ray &ray, const Scene &scene, Sampler &sampler,
              int depth = 0) const override;

 private:
  const int m_max_depth;
};
}  // namespace TRay