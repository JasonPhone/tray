#include "core/Integrator.h"
#include "core/geometry/Bound.h"
#include "core/Camera.h"
#include "core/Film.h"
#include "core/Sampler.h"

namespace TRay {

void SamplerIntegrator::render(const Scene &scene) {
  preprocess(scene, *m_sampler);
  // Render.
  // -------
  // Number of tiles.
  Bound2i sample_bound = m_camera->m_film->sample_bound();
  Vector2i sample_extent = sample_bound.diagonal();
  const int tile_size = 16;
  Point2i n_tiles((sample_extent.x + tile_size - 1) / tile_size,
                  (sample_extent.y + tile_size - 1) / tile_size);
  // Iteration.
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
        film_tile->add_sample(cam_sample.m_point_on_film, L, ray_w);
        // TODO Update GUI here?
      } while (tile_sampler->next_sample());
    }
    SInfo("SampleIntegrator::render:per_tile: Finished tile " +
          tile_bound.to_string());
    m_camera->m_film->merge_tile(std::move(film_tile));
  };
  // No parallelism by now.
  for (int y = 0; y < n_tiles.y; ++y)
    for (int x = 0; x < n_tiles.x; ++x) per_tile(Point2i(x, y));
  // Write to file.
  // --------------
  m_camera->m_film->write_image(1.0, nullptr);
}
} // namespace TRay