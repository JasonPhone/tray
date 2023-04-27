#pragma once
#include "core/TRay.h"
#include "core/Film.h"
#include "core/Camera.h"
#include "core/Sampler.h"

namespace TRay {
/// @brief Integrator interface.
class Integrator {
 public:
  virtual ~Integrator() {}
  virtual void render(const Scene &scene) = 0;
  virtual void render_step(const Scene &scene) = 0;
};

class SamplerIntegrator : public Integrator {
 public:
  SamplerIntegrator(std::shared_ptr<const Camera> &camera,
                    std::shared_ptr<Sampler> &sampler)
      : m_camera(camera), m_sampler(sampler) {}
  void render(const Scene &scene) override;
  void render_step(const Scene &scene) override;
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

 private:
  struct TileUnit {
    TileUnit(std::unique_ptr<Sampler> &sampler, std::unique_ptr<FilmTile> &tile,
             const Bound2i &tile_bound,
             const SamplerIntegrator &parent_integrator,
             const Scene &target_scene)
        : tile_sampler(std::move(sampler)),
          film_tile(std::move(tile)),
          tile_range(tile_bound),
          integrator(parent_integrator),
          scene(target_scene) {}
    std::unique_ptr<Sampler> tile_sampler = nullptr;
    std::unique_ptr<FilmTile> film_tile = nullptr;
    Bound2iIterator tile_range;
    const SamplerIntegrator &integrator;
    const Scene &scene;
    bool tile_done = false;
    int current_samples = 0;

    void render_one_sample() {
      if (tile_done) return;
      const Point2i &pxl = *tile_range;
      if (current_samples == 0) tile_sampler->start_pixel(pxl);
      // Get a camera sample.
      CameraSample cam_sample = tile_sampler->camera_sample(pxl);
      // Generate ray through this camera sample.
      Ray ray;
      Float ray_w = integrator.m_camera->ray_sample(cam_sample, &ray);
      // SDebug("ray_sample: " + ray.to_string());
      Spectrum L(0.0);
      if (ray_w > 0) L = integrator.Li(ray, scene, *tile_sampler);
      // Check.
      if (L.has_NaN()) {
        SError(string_format(
            "Get NaN radiance value "
            "for pixel (%d, %d), sample %d. Black value is used.",
            pxl.x, pxl.y, (int)tile_sampler->current_sample_index()));
        L = Spectrum(0.f);
      } else if (L.y() < -1e-5) {
        SError(string_format(
            "Get negative radiance value, %f "
            "for pixel (%d, %d), sample %d. Black value is used.",
            L.y(), pxl.x, pxl.y, (int)tile_sampler->current_sample_index()));
        L = Spectrum(0.f);
      } else if (std::isinf(L.y())) {
        SError(string_format(
            "Get INF radiance value "
            "for pixel (%d, %d), sample %d. Black value is used.",
            pxl.x, pxl.y, (int)tile_sampler->current_sample_index()));
        L = Spectrum(0.f);
      }
      film_tile->add_sample(cam_sample.m_point_film, L, ray_w);

      current_samples++;
      bool has_next_sample = tile_sampler->next_sample();
      if (!has_next_sample) {
        ++tile_range;
        current_samples = 0;
      }
      if (tile_range == tile_range.end()) {
        tile_done = true;
      }
      integrator.m_camera->m_film->merge_tile(*film_tile);
    }
  };
  std::vector<TileUnit> m_tiles;
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
Spectrum direct_lighting(const Interaction &inter, const Point2f &u_bsdf,
                         const Light &light, const Point2f &u_light,
                         const Scene &scene, Sampler &sampler,
                         bool do_specular = false);
}  // namespace TRay