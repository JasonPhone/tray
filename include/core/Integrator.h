#pragma once
#include "core/TRay.h"

namespace TRay {
/// @brief Integrator interface.
class Integrator {
 public:
  virtual ~Integrator() {}
  virtual void render(const Scene &scene, uint8_t *dst = nullptr) = 0;
};

class SamplerIntegrator : public Integrator {
 public:
  SamplerIntegrator(std::shared_ptr<const Camera> &camera,
                    std::shared_ptr<Sampler> &sampler)
      : m_camera(camera), m_sampler(sampler) {}
  void render(const Scene &scene, uint8_t *dst) override;
  virtual void preprocess(const Scene &scene, Sampler &sampler) {
    SInfo("SamplerIntegrator::preprocess: Start preprocessing.");
  }
  /// @brief Evaluate the incident radiance.
  /// @param ray The ray along which the radiance should be evaluated.
  /// @param scene The scene to be rendered.
  /// @param sampler The sample generator used by MCM to solve render equation.
  /// @param depth Number of ray bounces.
  /// @return
  virtual Spectrum Li(const Ray &ray, const Scene &scene, Sampler &sampler,
                      int depth = 0) const = 0;
  /// @brief Procedure function for specular reflection.
  Spectrum specular_reflect(const Ray &ray, const SurfaceInteraction &si,
                            const Scene &scene, Sampler &sampler,
                            int depth) const;
  /// @brief Procedure function for specular transmission.
  Spectrum specular_transmit(const Ray &ray, const SurfaceInteraction &si,
                             const Scene &scene, Sampler &sampler,
                             int depth) const;

 protected:
 private:
  std::shared_ptr<const Camera> m_camera;
  std::shared_ptr<Sampler> m_sampler;
};

/// @brief Sample all light sources uniformly.
/// @param inter Point to be lit.
/// @param n_samples Number of samples for each light.
Spectrum light_sample_uniform_all(const Interaction &inter, const Scene &scene,
                                  Sampler &sampler,
                                  const std::vector<int> &n_samples);
/// @brief Take one sample from one light source randomly.
/// @param inter Point to be lit.
/// @param dist_1D Used for special light source selecting.
Spectrum light_sample_uniform_one(const Interaction &inter, const Scene &scene,
                                  Sampler &sampler,
                                  Distribution1D *dist_1D = nullptr);
/// @brief Estimate the dirct lighting using MIS of BSDF and light source.
/// @param do_specular true to process perfect specular.
Spectrum direct_lighting(const Interaction &inter, const Point2f &u_scatter,
                         const Light &light, const Point2f &u_light,
                         const Scene &scene, Sampler &sampler,
                         bool do_specular = false);
}  // namespace TRay