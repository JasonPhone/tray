#include "integrators/PathIntegrator.h"
#include "core/spectrum/spectrum.h"
#include "core/geometry/geometry.h"
#include "core/Scene.h"
#include "core/reflection/BSDF.h"
#include "core/Sampler.h"

namespace TRay {
Spectrum PathIntegrator::Li(const Ray &ray, const Scene &scene,
                            Sampler &sampler, int depth) const {
  // SDebug("path integrator Li begin");
  // Sum of radiance of all sub paths until the longest one.
  // The paths in one PathIntegrator::Li() is from one point sequence.
  Spectrum L(0.0);
  // Weight for path throughput of last length.
  Spectrum thorughput_factor(1.0);
  // Next ray, to find the next point and extend the path.
  // The first next is current ray.
  Ray ray_nxt(ray);
  // true if the last ray is from a specular reflection.
  bool from_specular = false;
  int bounce_cnt = 0;
  // TODO Improve the multi-refraction problem.
  // // Track the scale on throughput factor
  // // and remove it when doing Russian roulette.
  // Float refract_scale = 1;
  for (bounce_cnt = 0;; bounce_cnt++) {
    // SDebug(string_format("start bounce %d ", bounce_cnt + 1));
    // Extend the path one more point and add the contribution.
    // --------------------------------------------------------
    // Trace the ray for an intersection.
    SurfaceInteraction si;
    bool hitted = scene.intersect(ray_nxt, &si);
    /**
     * If ray hits an emissive object, emitted light is often ignored,
     * since a shorter path has considered this as its endpoint and
     * performed direct lighting.
     * Except for: 1) the initial bounce 2) current ray is from a
     * delta distribution, which is ignored by direct lighting.
     */
    // If emissive should be considered..
    if (bounce_cnt == 0 || from_specular) {
      // Emitted light.
      if (hitted) {
        // SDebug("emissive and hitted");
        L += thorughput_factor * si.Le(-ray_nxt.dir);
      } else {
        // SDebug("emissive but no hit");
        for (const auto &light : scene.m_lights) {
          L += thorughput_factor * light->Le(ray_nxt);
        }
      }
    }
    // Cut the path if no intersection or depth exceeded.
    if (!hitted || bounce_cnt >= m_max_depth) {
      // SDebug("no hit or depth exceeded, quit current Li");
      break;
    }
    // Fill BSDF and skip for null bsdf intersection (media stuff).
    // SDebug("hitted, fill BSDF");
    si.fill_scattering_func(ray, TransportMode::Radiance, true);
    if (si.bsdf == nullptr) {
      // SDebug("empty bsdf, skip this bounce");
      ray_nxt = si.ray_along(ray_nxt.dir);
      bounce_cnt--;
      continue;
    }
    // Sample from light to find the contribution. Skip perfect specular BxDFs.
    if (si.bsdf->num_BxDFs(BxDFType(BSDF_ALL & ~BSDF_SPECULAR)) > 0) {
      // SDebug("have non-specular BSDF, evaluating");
      Spectrum LL =
          thorughput_factor * light_sample_uniform_one(si, scene, sampler);
      // SDebug("got " + LL.to_string());
      L += LL;
    }
    // Prepare for next bounce.
    // ------------------------
    // Sample the BSDF to shoot a new ray.
    // SDebug("sample BSDF for next ray");
    Vector3f wo = -ray_nxt.dir, wi;
    Float pdf_val;
    BxDFType sampled_type;
    Spectrum f = si.bsdf->sample_f(wo, &wi, sampler.sample_2D(), &pdf_val,
                                   BSDF_ALL, &sampled_type);
    if (f.is_black() || pdf_val == 0.0) {
      // SDebug("0 BSDF value or pdf value, quit current Li");
      break;
    }
    thorughput_factor *= f * abs_dot(si.shading.n, wi) / pdf_val;
    from_specular = (sampled_type & BSDF_SPECULAR) != 0;
    ray_nxt = si.ray_along(wi);
    // Russian roulette, bounce after 3 is possibly terminated.
    if (bounce_cnt > 3) {
      // We hope this represents the illuminance.
      Float illumi = thorughput_factor.max_component();
      illumi /= 3;
      Float p = std::max(0.05, 1 - illumi);
      if (sampler.sample_1D() < p) {
        // SDebug("roulette BANG! quit current Li");
        break;
      }
      thorughput_factor /= (1 - p);
    }
    delete si.bsdf;
  }
  // SDebug("current Li returned " + L.to_string() + "\n");
  return L;
}
}  // namespace TRay