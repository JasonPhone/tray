#include "integrators/WhittedIntegrator.h"
#include "core/spectrum/spectrum.h"
#include "core/geometry/Interaction.h"
#include "core/Light.h"
#include "core/Scene.h"
#include "core/Sampler.h"
#include "core/reflection/BSDF.h"

namespace TRay {

Spectrum WhittedIntegrator::Li(const Ray &ray, const Scene &scene,
                               Sampler &sampler, int depth) const {
  Spectrum L(0.0);
  // Find intersection.
  // ------------------
  SurfaceInteraction si;
  if (!scene.intersect(ray, &si)) {
    // Environment lighting.
    for (const auto &light : scene.m_lights) L += light->Le(ray);
    return L;
  }
  // Now we got a hit.
  // Local shading.
  // ---------------------
  // Prepare for Whitted.
  Normal3f n = si.shading.n;
  Vector3f wo = si.wo;
  // Fill BSDF.
  si.fill_scattering_func(ray);
  if (si.bsdf == nullptr)
    // Skip current intersection.
    return Li(si.ray_along(ray.dir), scene, sampler, depth);
  // Self emissive.
  L += si.Le(wo);
  // Contribution of light source.
  for (const auto &light : scene.m_lights) {
    // Normalized, from interaction point to point on light.
    Vector3f wi;
    Float pdf;
    VisibilityTester visibility;
    // Li assuming no occlusion.
    Spectrum Li =
        light->sample_Li(si, sampler.sample_2D(), &wi, &pdf, &visibility);
    if (Li.is_black() || pdf == 0) continue;
    // Shading.
    Spectrum f = si.bsdf->f(wo, wi);
    if (!f.is_black() && !visibility.blocked(scene)) {
      L += f * Li * abs_dot(wi, n) / pdf;
    }
  }
  // Tracing for specular reflection and transmission.
  // -------------------------------------------------
  if (depth + 1 < m_max_depth) {
    // Specular reflect.
    // SDebug("specular reflect");
    L += specular_reflect(ray, si, scene, sampler, depth);
    // Specular transmit.
    // SDebug("specular transmit");
    L += specular_transmit(ray, si, scene, sampler, depth);
  }

  return L;
}
}  // namespace TRay