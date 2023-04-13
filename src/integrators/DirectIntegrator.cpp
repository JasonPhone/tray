#include "core/Integrator.h"
#include "core/Sampler.h"
#include "core/Scene.h"
#include "integrators/DirectIntegrator.h"
#include "core/spectrum/spectrum.h"
#include "core/geometry/Interaction.h"
#include "core/reflection/BSDF.h"

namespace TRay {
void DirectIntegrator::preprocess(const Scene &scene, Sampler &sampler) {
  SInfo("DirectIntegrator::preprocess: Start preprocessing.");
  // Request sample arrays if using LightSample::UNIFORM_ALL
  // Preprocess is done once Integrator::render() is called,
  // so not all rays can get sufficient sample array, and will
  // take only one sample for each light.
  m_n_light_samples.clear();
  if (m_light_sample == LightSample::UNIFORM_ALL) {
    // Number of lights for each light.
    for (const auto &light : scene.m_lights)
      m_n_light_samples.push_back(sampler.round(light->m_n_samples));
    // Request arrays, each recursion and each light.
    for (int i = 0; i < m_max_depth; i++) {
      for (const auto &nsamples : m_n_light_samples) {
        // Reflect and transmit each.
        sampler.request_2D_array(nsamples);
        sampler.request_2D_array(nsamples);
      }
    }
  }
}
Spectrum DirectIntegrator::Li(const Ray &ray, const Scene &scene,
                              Sampler &sampler, int depth = 0) const {
  Spectrum L(0.0);
  // Find Intersection.
  // ------------------
  SurfaceInteraction si;
  if (!scene.intersect(ray, &si)) {
    for (const auto &light : scene.m_lights) L += light->Le(ray);
    return L;
  }
  // Local shading.
  // --------------
  // Fill BSDF.
  si.fill_scattering_func(ray);
  if (si.bsdf == nullptr)
    return Li(si.ray_along(ray.dir), scene, sampler, depth);
  // Self emissive.
  Normal3f n = si.shading.n;
  Vector3f wo = si.wo;
  L += si.Le(wo);
  // **Direct lighting**
  if (!scene.m_lights.empty()) {
    if (m_light_sample == LightSample::UNIFORM_ALL) {
      L += light_sample_uniform_all(si, scene, sampler, m_n_light_samples);
    } else {
      L += light_sample_uniform_one(si, scene, sampler);
    }
  }
  // Tracing for specular reflection and transmission.
  // -------------------------------------------------
  if (depth + 1 < m_max_depth) {
    L += specular_reflect(ray, si, scene, sampler, depth);
    L += specular_transmit(ray, si, scene, sampler, depth);
  }
  return L;
}
}  // namespace TRay