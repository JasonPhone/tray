#pragma once
#include "core/TRay.h"

namespace TRay {
/// @brief Integrator interface.
class Integrator {
 public:
  virtual ~Integrator() {}
  virtual void render(const Scene &scene) = 0;
};

class SamplerIntegrator : public Integrator {
 public:
  SamplerIntegrator(std::shared_ptr<const Camera> &camera,
                    std::shared_ptr<Sampler> &sampler)
      : m_camera(camera), m_sampler(sampler) {}
  void render(const Scene &scene) override;
  virtual void preprocess(const Scene &scene, Sampler &sampler) {}
  /// @brief Evaluate the incident radiance.
  /// @param ray The ray along which the radiance should be evaluated.
  /// @param scene The scene to be rendered.
  /// @param sampler The sample generator used by MCM to solve render equation.
  /// @param depth Number of ray bounces.
  /// @return
  virtual Spectrum Li(const Ray &ray, const Scene &scene, Sampler &sampler,
                      int depth = 0) const = 0;
  Spectrum specular_reflect(const Ray &ray, const SurfaceInteraction &si,
                            const Scene &scene, Sampler &sampler,
                            int depth) const;
  Spectrum specular_transmit(const Ray &ray, const SurfaceInteraction &si,
                             const Scene &scene, Sampler &sampler,
                             int depth) const;

 protected:
 private:
  std::shared_ptr<const Camera> m_camera;
  std::shared_ptr<Sampler> m_sampler;
};
}  // namespace TRay