#pragma once
#include "core/TRay.h"
#include "core/geometry/Point.h"
#include "core/geometry/Bound.h"
#include "core/Filter.h"
#include "core/spectrum/spectrum.h"

namespace TRay {
/// @brief Sum of 'weight * value' and sum of weight.
struct FilmTilePixel {
  Spectrum contrib_sum{0};
  Float filter_weight_sum = 0.0;
};
/// @brief Used by Film.
struct Pixel {
  // Weighted RGB sum of this pixel.
  // TODO XYZ would be better. Spectrum is too much for output.
  Float rgb[3] = {0, 0, 0};
  Float filter_weight_sum = 0;
};

class Film {
 public:
  /**
   * @brief Construct a new Film object.
   *
   * @param resolution Full resolution of image.
   * @param crop_window Part of image to be stored and written, NDC space.
   * @param filter Filter used to recover pixel values from samples.
   * @param filename
   */
  Film(const Point2i &resolution, const Bound2f &crop_window,
       std::unique_ptr<Filter> filter, const std::string &filename);
  /// @brief Get the area to be sampled.
  Bound2i sample_bound() const;
  // Bound2f physical_extent() const; // The physical area of film.
  std::unique_ptr<FilmTile> get_tile(const Bound2i &tile_bound);
  /// @brief Merge this tile into the film.
  ///        Note that the ownership is transferred.
  void merge_tile(std::unique_ptr<FilmTile> tile);
  /// @brief Set the whole (cropped) image.
  /// @param colors Spectrum array. The size should be equal to image area.
  void set_image(const Spectrum *colors);
  /// @brief Write image as RGBRGBRGB... into dst.
  void write_image(Float, uint8_t *dst);

  Point2i m_full_resolution;
  std::unique_ptr<Filter> m_filter;
  const std::string m_filename;
  Bound2i m_cropped_pixel_bound;

 private:
  Pixel &pixel(const Point2i &p);
  // Pointer to the pixel array.
  std::unique_ptr<Pixel[]> m_pixels;
  static constexpr int filter_table_width = 16;
  /// @brief 1/4 part of the filter table, assuming that the other 3 parts are
  /// symmertric. The precision error of position is not significant.
  Float m_filter_table[filter_table_width * filter_table_width];
};

class FilmTile {
 public:
  FilmTile(const Bound2i &pixel_bound, const Vector2f &filter_radius,
           const Float *filter_tbl, int filter_tbl_width)
      : m_pixel_bound(pixel_bound),
        m_filter_radius(filter_radius),
        m_filter_radius_inv(1.0 / filter_radius.x, 1.0 / filter_radius.y),
        m_filter_table(filter_tbl),
        m_filter_table_width(filter_tbl_width) {
    m_pixels = std::vector<FilmTilePixel>(std::max(0, m_pixel_bound.area()));
  }

  void add_sample(const Point2f &point_film, const Spectrum &L,
                  Float sample_weight);
  /// @brief Get the pixel in film tile.
  /// @param p Position in film space.
  FilmTilePixel &pixel(const Point2i &p);
  const Bound2i &tile_bound() const;

 private:
  const Bound2i m_pixel_bound;
  const Vector2f m_filter_radius, m_filter_radius_inv;
  const Float *m_filter_table;
  const int m_filter_table_width;
  std::vector<FilmTilePixel> m_pixels;
};
}  // namespace TRay