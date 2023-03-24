#include "core/Film.h"
#include "core/imageio.h"

namespace TRay {
Film::Film(const Point2i &resolution, const Bound2f &crop_window,
           std::unique_ptr<Filter> filter, Float diagonal,
           const std::string &filename, Float scale)
    : m_full_resolution(resolution),
      m_diagonal(diagonal),
      m_filter(std::move(filter)),
      m_filename(filename),
      m_scale(scale) {
  // Calculate the actually stored image.
  m_cropped_pixel_bound =
      Bound2i(Point2i(std::ceil(m_full_resolution.x * crop_window.p_min.x),
                      std::ceil(m_full_resolution.y * crop_window.p_min.y)),
              Point2i(std::ceil(m_full_resolution.x * crop_window.p_max.x),
                      std::ceil(m_full_resolution.y * crop_window.p_max.y)));
  // Allocate the pixel array.
  m_pixels = std::unique_ptr<Pixel[]>(new Pixel[m_cropped_pixel_bound.area()]);
  // Precompute filter LUT.
  int offset = 0;
  Point2f p;
  for (int y = 0; y < filter_table_width; y++) {
    for (int x = 0; x < filter_table_width; x++, offset++) {
      p.x = (x + 0.5) / filter_table_width * m_filter->m_radius.x;
      p.y = (y + 0.5) / filter_table_width * m_filter->m_radius.y;
      m_filter_table[offset] = m_filter->evaluate(p);
    }
  }
}
Bound2i Film::sample_bound() const {
  // The 0.5 offser is to move to the center.
  Bound2f bound_float(floor(Point2f(m_cropped_pixel_bound.p_min) +
                            Vector2f(0.5, 0.5) - m_filter->m_radius),
                      ceil(Point2f(m_cropped_pixel_bound.p_max) -
                           Vector2f(0.5, 0.5) + m_filter->m_radius));
  return (Bound2i)bound_float;
}
Pixel &Film::pixel(const Point2i &p) {
  Bound2i &bound = m_cropped_pixel_bound;
  ASSERT(point_in_bound_open(p, bound));
  int xwidth = bound.p_max.x - bound.p_min.x;
  int offset = xwidth * (p.y - bound.p_min.y) + (p.x - bound.p_min.x);
  return m_pixels[offset];
}
void Film::merge_tile(std::unique_ptr<FilmTile> tile) {
  // TODO Need lock here if using multi-therads.
  Bound2iIterator pxl_range(tile->tile_bound());
  for (const Point2i &pxl_pos : pxl_range) {
    const FilmTilePixel &tile_pxl = tile->pixel(pxl_pos);
    Pixel &dst_pxl = this->pixel(pxl_pos);
    dst_pxl.rgb[0] += tile_pxl.contrib_sum[0];
    dst_pxl.rgb[1] += tile_pxl.contrib_sum[1];
    dst_pxl.rgb[2] += tile_pxl.contrib_sum[2];
    dst_pxl.filter_weight_sum += tile_pxl.filter_weight_sum;
  }
}
void Film::set_image(const Spectrum *colors) {
  int n_pixels = m_cropped_pixel_bound.area();
  for (int i = 0; i < n_pixels; i++) {
    Pixel &p = m_pixels[i];
    p.rgb[0] = colors[i][0];
    p.rgb[1] = colors[i][1];
    p.rgb[2] = colors[i][2];
    p.filter_weight_sum = 1.0;
  }
}
void Film::write_image(Float, uint8_t *dst) {
  ASSERT(dst != nullptr);
  int offset = 0;
  Float vals[m_cropped_pixel_bound.area() * 3];
  for (int i = 0; i < m_cropped_pixel_bound.area(); i++) {
    Pixel pxl = m_pixels[i];
    vals[offset * 3 + 0] = pxl.rgb[0];
    vals[offset * 3 + 1] = pxl.rgb[1];
    vals[offset * 3 + 2] = pxl.rgb[2];
    if (pxl.filter_weight_sum) {
      Float w_inv = 1.0 / pxl.filter_weight_sum;
      vals[offset * 3 + 0] = std::max(Float(0), vals[offset * 3 + 0] * w_inv);
      vals[offset * 3 + 1] = std::max(Float(0), vals[offset * 3 + 1] * w_inv);
      vals[offset * 3 + 2] = std::max(Float(0), vals[offset * 3 + 2] * w_inv);
    }
// Gamma correction and scale to [0, 255].
#define uint8RGB(v) (uint8_t) clamp(255.0 * gamma_correct(v) + 0.5, 0.0, 255.0)
    dst[offset * 3 + 0] = uint8RGB(vals[offset * 3 + 0]);
    dst[offset * 3 + 1] = uint8RGB(vals[offset * 3 + 1]);
    dst[offset * 3 + 2] = uint8RGB(vals[offset * 3 + 2]);
#undef uint8RGB(v)
    offset++;
  }
}

void FilmTile::add_sample(const Point2f &point_on_film, const Spectrum &L,
                          Float sample_weight) {
  // Find the influenced pixels.
  /**
   * The overall procedure is:
   *    1. Expand by radius
   *    2. Offset the range by -0.5
   *    3. Pixels with center (x.5) still in the range will be influenced by
   *       this sample. d = floor(c) is done here.
   */
  Point2f p_discrete = point_on_film - Vector2f(0.5, 0.5);
  Point2i p0 = (Point2i)ceil(p_discrete - m_filter_radius);
  // Open range.
  Point2i p1 = (Point2i)floor(p_discrete + m_filter_radius) + Vector2i(1, 1);
  // Crop.
  p0 = max(p0, m_pixel_bound.p_min);
  p1 = min(p1, m_pixel_bound.p_max);
  // Precompute offset in the table.
  int *offx = ALLOCA(int, p1.x - p0.x);
  for (int x = p0.x; x < p1.x; x++) {
    Float fx = std::abs((x - p_discrete.x)) * m_filter_radius_inv *
               m_filter_table_width;
    offx[x - p0.x] = std::min((int)std::floor(fx), m_filter_table_width);
  }
  int *offy = ALLOCA(int, p1.y - p0.y);
  for (int y = p0.y; y < p1.y; y++) {
    Float fy = std::abs((y - p_discrete.y)) * m_filter_radius_inv *
               m_filter_table_width;
    offy[y - p0.y] = std::min((int)std::floor(fy), m_filter_table_width);
  }
  for (int y = p0.y; y < p1.y; y++) {
    for (int x = p0.x; x < p1.x; x++) {
      int offset = offy[y - p0.y] * m_filter_table_width + offx[x - p0.x];
      Float filter_weight = m_filter_table[offset];
      FilmTilePixel &pxl = pixel(Point2i(x, y));
      pxl.contrib_sum += L * sample_weight * filter_weight;
      pxl.filter_weight_sum = filter_weight;
    }
  }
}
FilmTilePixel &FilmTile::pixel(const Point2i &p) {
  ASSERT(point_in_bound_open(p, m_pixel_bound));
  int xwidth = m_pixel_bound.p_max.x - m_pixel_bound.p_min.x;
  int offset =
      xwidth * (p.y - m_pixel_bound.p_min.y) + (p.x - m_pixel_bound.p_min.x);
  return m_pixels[offset];
}
Bound2i FilmTile::tile_bound() const { return m_pixel_bound; }
}  // namespace TRay