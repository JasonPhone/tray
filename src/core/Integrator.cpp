#include "core/Integrator.h"
#include "core/geometry/Bound.h"
#include "core/Camera.h"
#include "core/Film.h"
#include "core/Sampler.h"
#include "core/Scene.h"
#include "core/geometry/Interaction.h"
#include "core/reflection/BSDF.h"
#include "core/reflection/BxDF.h"
#include "core/math/sampling.h"

namespace TRay {
void SamplerIntegrator::render(const Scene &scene) {
  SInfo("SamplerIntegrator::render: Start rendering.");
  preprocess(scene, *m_sampler);
  // Render.
  // -------
  // Number of tiles.
  Bound2i sample_bound = m_camera->m_film->sample_bound();
  Vector2i sample_extent = sample_bound.diagonal();
  const int tile_size = 16;
  int tile_cnt = 0;
  Point2i n_tiles((sample_extent.x + tile_size - 1) / tile_size,
                  (sample_extent.y + tile_size - 1) / tile_size);
  SInfo("SampleIntegrator::render:\n\tSample bound: " +
        sample_bound.to_string());
  // Iteration.
  /**
   * For every tile:
   *   Get the tile coords.
   *   Compute the tile extent.
   *   Get the FilmTile.
   *   Loop over pixels in this tile.
   *     Loop over samples in this pixel.
   *       Generate camera sample.
   *       Get the Li.
   *       Add Li to FilmTile.
   *   Merge the FilmTile.
   */
  auto per_tile = [&](Point2i tile) {
    // Memory allocation.
    // TODO MemoryPool
    // Sample instance for this tile.
    int sample_seed = tile.y * n_tiles.x + tile.x;
    std::unique_ptr<Sampler> tile_sampler = m_sampler->clone(sample_seed);
    // Bound for this tile.
    int x0 = sample_bound.p_min.x + tile.x * tile_size;
    int x1 = std::min(x0 + tile_size, sample_bound.p_max.x);
    int y0 = sample_bound.p_min.y + tile.y * tile_size;
    int y1 = std::min(y0 + tile_size, sample_bound.p_max.y);
    Bound2i tile_bound(Point2i(x0, y0), Point2i(x1, y1));
    // Take tile from film.
    std::unique_ptr<FilmTile> film_tile =
        m_camera->m_film->get_tile(tile_bound);
    // Loop over pixels in this FilmTile.
    SInfo(string_format("SampleIntegrator::render: %d/%d \n\tBegin tile ",
                        tile_cnt, n_tiles.x * n_tiles.y) +
          tile_bound.to_string());
    Bound2iIterator bound_range(tile_bound);
    for (const Point2i &pxl : bound_range) {
      // Begin for this pixel.
      tile_sampler->start_pixel(pxl);
      do {
        // Get a camera sample.
        CameraSample cam_sample = tile_sampler->camera_sample(pxl);
        // Generate ray through this camera sample.
        Ray ray;
        Float ray_w = m_camera->ray_sample(cam_sample, &ray);
        // SDebug("ray_sample: " + ray.to_string());
        Spectrum L(0.0);
        if (ray_w > 0) L = Li(ray, scene, *tile_sampler);
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
      } while (tile_sampler->next_sample());
    }
    m_camera->m_film->merge_tile(std::move(film_tile));
  };
  // No parallelism by now.
  for (int y = 0; y < n_tiles.y; ++y)
    for (int x = 0; x < n_tiles.x; ++x) {
      tile_cnt++;
      per_tile(Point2i(x, y));
    }
  // Write to file.
  // --------------
  SInfo("SamplerIntegrator::render: Done rendering.");
}
/**
 * @brief Basic design for step progressive rendering.
 *
 * Procedure:
 *  1. Get every tile ready.
 *     Need to record their status.
 *  2. Loop over every tile.
 *     Do one pixel of samples for this tile,
 *     if have unfinished pixels.
 *  3. Update the film.
 *
 * Notes:
 *  A caller should be able to call a step rendering
 *  and get a result image with every tile having one
 *  pixel updated. Which means the privided function should
 *  just push a step further, better return a step count and
 *  all-finished flag.
 *  The move sematics of Film::merge_tile. It should be changed.
 */
bool SamplerIntegrator::render_step(const Scene &scene) {
  if (m_tiles.empty()) {
    SInfo("SamplerIntegrator::render_step: Empty tile list, preprocessing.");
    preprocess(scene, *m_sampler);

    // Number of tiles.
    Bound2i sample_bound = m_camera->m_film->sample_bound();
    Vector2i sample_extent = sample_bound.diagonal();
    const int tile_size = 4;
    Point2i n_tiles((sample_extent.x + tile_size - 1) / tile_size,
                    (sample_extent.y + tile_size - 1) / tile_size);
    SInfo("SampleIntegrator::render_step:\n\tSample bound: " +
          sample_bound.to_string());
    for (int y = 0; y < n_tiles.y; ++y)
      for (int x = 0; x < n_tiles.x; ++x) {
        int sample_seed = y * n_tiles.x + x;
        std::unique_ptr<Sampler> tile_sampler = m_sampler->clone(sample_seed);
        // Bound for this tile.
        int x0 = sample_bound.p_min.x + x * tile_size;
        int x1 = std::min(x0 + tile_size, sample_bound.p_max.x);
        int y0 = sample_bound.p_min.y + y * tile_size;
        int y1 = std::min(y0 + tile_size, sample_bound.p_max.y);
        Bound2i tile_bound(Point2i(x0, y0), Point2i(x1, y1));
        // Take tile from film.
        std::unique_ptr<FilmTile> film_tile =
            m_camera->m_film->get_tile(tile_bound);
        // Loop over pixels in this FilmTile.
        m_tiles.push_back(
            TileUnit{tile_sampler, film_tile, tile_bound, *this, scene});
      }
    SInfo(string_format("Generated %d tiles.", int(m_tiles.size())));
    return false;
  } else {
    bool done = true;
    for (auto &tile : m_tiles) {
      if (tile.tile_done) continue;
      tile.render_one_sample();
      done = false;
    }
    return done;
  }
}
Spectrum SamplerIntegrator::specular_reflect(const Ray &ray,
                                             const SurfaceInteraction &si,
                                             const Scene &scene,
                                             Sampler &sampler,
                                             int depth) const {
  Vector3f wo = si.wo, wi;
  Float pdf_val;
  // Specular reflection direction and BSDF value.
  Spectrum f = si.bsdf->sample_f(wo, &wi, sampler.sample_2D(), &pdf_val,
                                 BxDFType(BSDF_SPECULAR | BSDF_REFLECTION));
  // Contribution of specular reflection.
  const Normal3f &ns = si.shading.n;
  Spectrum L(0.0);
  if (pdf_val > 0 && !f.is_black() && abs_dot(ns, wi) != 0)
    L = f * Li(ray, scene, sampler, depth + 1) * abs_dot(ns, wi) / pdf_val;
  // SDebug("integrator: got spectrum L " + L.to_string());
  return L;
}
Spectrum SamplerIntegrator::specular_transmit(const Ray &ray,
                                              const SurfaceInteraction &si,
                                              const Scene &scene,
                                              Sampler &sampler,
                                              int depth) const {
  Vector3f wo = si.wo, wi;
  Float pdf_val;
  // Specular reflection direction and BSDF value.
  Spectrum f = si.bsdf->sample_f(wo, &wi, sampler.sample_2D(), &pdf_val,
                                 BxDFType(BSDF_SPECULAR | BSDF_TRANSMISSION));
  // Contribution of specular reflection.
  const Normal3f &ns = si.shading.n;
  Spectrum L(0.0);
  if (pdf_val > 0 && !f.is_black() && abs_dot(ns, wi) != 0)
    L = f * Li(ray, scene, sampler, depth + 1) * abs_dot(ns, wi) / pdf_val;
  return L;
}

Spectrum light_sample_uniform_all(const Interaction &inter, const Scene &scene,
                                  Sampler &sampler,
                                  const std::vector<int> &n_samples) {
  Spectrum L(0.0);
  ASSERT(scene.m_lights.size() == n_samples.size());
  for (size_t i = 0; i < scene.m_lights.size(); i++) {
    // Prepare light and sample array.
    const std::shared_ptr<Light> &light = scene.m_lights[i];
    int sample_num = n_samples[i];
    const Point2f *u_light_arr = sampler.get_2D_array(sample_num);
    const Point2f *u_bsdf_arr = sampler.get_2D_array(sample_num);
    if (u_light_arr == nullptr || u_bsdf_arr == nullptr) {
      // Arrays run out.
      Point2f u_light = sampler.sample_2D();
      Point2f u_bsdf = sampler.sample_2D();
      // Not handeling specuar here, they are indirect.
      L += direct_lighting(inter, u_bsdf, *light, u_light, scene, sampler);
    } else {
      Spectrum L_direct(0.0);
      for (int j = 0; j < sample_num; j++) {
        L_direct += direct_lighting(inter, u_bsdf_arr[j], *light,
                                    u_light_arr[j], scene, sampler);
      }
      L += L_direct / sample_num;
    }
  }
  return L;
}
Spectrum light_sample_uniform_one(const Interaction &inter, const Scene &scene,
                                  Sampler &sampler, Distribution1D *dist_1D) {
  // SDebug("sample one of lights");
  if (scene.m_lights.empty()) {
    // SDebug("no lights, return black");
    return Spectrum(0.0);
  }
  Spectrum L(0.0);
  int n_lights = int(scene.m_lights.size());
  int light_idx = 0;
  Float light_pdf = 0.0;
  // Select a light source.
  if (dist_1D) {
    light_idx = dist_1D->sample_discrete(sampler.sample_1D(), &light_pdf);
  } else {
    light_idx = clamp(int(sampler.sample_1D() * n_lights), 0, n_lights - 1);
    light_pdf = 1.0 / n_lights;
  }
  // SDebug(string_format("seleted light %d from %d lights", light_idx,
  // n_lights)); Compute lighting.
  if (light_pdf > 0) {
    const std::shared_ptr<Light> &light = scene.m_lights[light_idx];
    L = direct_lighting(inter, sampler.sample_2D(), *light, sampler.sample_2D(),
                        scene, sampler);
  }
  // SDebug("light sampling done");
  return L / light_pdf;
}

Spectrum direct_lighting(const Interaction &inter, const Point2f &u_bsdf,
                         const Light &light, const Point2f &u_light,
                         const Scene &scene, Sampler &sampler,
                         bool do_specular) {
  // SDebug("===========================\ndirect lighting begin");
  BxDFType flags =
      do_specular ? BSDF_ALL : BxDFType(BSDF_ALL & (~BSDF_SPECULAR));
  Spectrum Ld(0.0);  // Final result.
  Float light_pdf = 0, bsdf_pdf = 0;
  Vector3f wi;
  // Light term and BSDF term.
  Spectrum Li(0.0), f(0.0);
  /**
   * estimator =
   *  1/nLi   sum( f(x) Li(x) WLi(x) / pdfLi(x) ) +
   *  1/nBSDF sum( f(y) Li(y) Wf(y)  / pdff(y)  )
   *
   * We are dealing just one item in the sum.
   * We have a sample of wi and need to choose Li or f to evaluate first.
   * The order matters since one of them may often return small value,
   * a nearly specular material and a big light for example.
   */
  // MIS for light sources, Li goes first.
  // SDebug("MIS for Li");
  VisibilityTester vis;
  Li = light.sample_Li(inter, u_light, &wi, &light_pdf, &vis);
  if (light_pdf > 0 && !Li.is_black()) {
    // SDebug("non-zero pdf and non-black Li");
    //  BSDF value for this light sample Li.
    if (inter.is_surface_interaction()) {
      // SDebug("surface interaction, evaluate BSDF");
      //  Safe cast.
      const SurfaceInteraction &si = (const SurfaceInteraction &)inter;
      f = si.bsdf->f(si.wo, wi, flags) * abs_dot(wi, si.shading.n);
      bsdf_pdf = si.bsdf->pdf(si.wo, wi, flags);
    }
    if (!f.is_black()) {
      // SDebug("non-zero BSDF value");
      //  Visibility test.
      // SDebug("test blocking");
      if (vis.blocked(scene)) {
        // SDebug("this light is blocked by others! setting Li to black");
        Li = Spectrum(0.0);
      }
      // SDebug("test black");
      if (!Li.is_black()) {
        // SDebug("non-black Li");
        //  Add contribution.
        if (light.is_delta_light()) {
          // SDebug("delta light");
          Ld += f * Li / light_pdf;
        } else {
          // SDebug("non-delta light");
          Float weight = power_heuristic(1, light_pdf, 1, bsdf_pdf);
          Ld += f * Li * weight / light_pdf;
        }
      }
    }
  }
  // Light sources with delta distribution is
  // never likely to be sampled with "that" direction.
  // SDebug("MIS for BSDF");
  if (!light.is_delta_light()) {
    // SDebug("non-delta light, can sample f");
    //  MIS for BSDF, f goes first.
    bool specular_sampled = false;
    f = Spectrum(0.0);
    Li = Spectrum(0.0);
    if (inter.is_surface_interaction()) {
      // Sample the direction wi.
      // SDebug("surface interaction, sample BSDF");
      BxDFType sampled_type;
      const SurfaceInteraction &si = (const SurfaceInteraction &)inter;
      f = si.bsdf->sample_f(si.wo, &wi, u_bsdf, &bsdf_pdf, flags,
                            &sampled_type);
      f *= abs_dot(wi, si.shading.n);
      specular_sampled = (sampled_type & BSDF_SPECULAR) != 0;
    }
    if (bsdf_pdf > 0 && !f.is_black()) {
      // SDebug("non-zero pdf and non-zero bsdf");
      //  Light contribution along wi.
      Float weight = 1;
      if (!specular_sampled) {  // Delta distribution cannot be sampled.
        // SDebug("non-specular sample, evaluating Li");
        light_pdf = light.pdf_Li(inter, wi);
        // Light term is not sampled.
        if (light_pdf == 0) {
          // SDebug("no chance to hit light, returning");
          // SDebug("---------------------------");
          return Ld;
        }
        weight = power_heuristic(1, bsdf_pdf, 1, light_pdf);
      }
      // If the direction sampled from BSDF hits the light.
      // SDebug("shoot the light");
      SurfaceInteraction light_si;
      Ray ray = inter.ray_along(wi);
      bool hitted = scene.intersect(ray, &light_si);
      // Add contribution.
      if (hitted) {
        // SDebug("hitted some light");
        if (light_si.primitive->area_light() == &light) {
          // SDebug("is current light");
          Li = light_si.Le(-wi);
        }
      } else {
        // SDebug("no hit, evaluating environment light");
        Li = light.Le(ray);
      }
      if (!Li.is_black()) {
        // SDebug("non-black Li");
        Ld += f * Li * weight / bsdf_pdf;
      }
    }
  }
  // SDebug("done direct lighting, Ld is " + Ld.to_string());
  // SDebug("---------------------------");
  return Ld;
}
}  // namespace TRay