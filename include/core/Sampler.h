#include "core/TRay.h"
#include "core/geometry/Point.h"
#include "core/Camera.h"
#include "core/math/RNG.h"

namespace TRay {
class Sampler {
 public:
  Sampler(int64_t sample_per_pixel);
  /// @brief Start sampling on a new pixel.
  /// @param p Position of the raster 'pixel'.
  virtual void start_pixel(const Point2i &p);
  /// @brief Get one (1D) value from current sample vector.
  virtual Float sample_1D() = 0;
  /// @brief Get two (2D) continuous values from current sample vector.
  ///        The correlation of dimensions is useful for some samplers.
  virtual Point2f sample_2D() = 0;
  /// @brief Get a camera sample in one time. Consumes 5 samples in total.
  /// @param p Position of the raster 'pixel'.
  CameraSample camera_sample(const Point2i &p);
  /// @brief Spawn samples in order of dimension-first.
  /// @param n Number of values for each sample. Better with Sampler::round().
  void request_1D_array(int n);
  /// @brief Get the requested array of sample vectors.
  /// @param n Number of samples for verification.
  /// @return Pointer to the first sample.
  const Float *get_1D_array(int n);
  /// @brief Spawn samples in order of dimension-first.
  /// @param n Number of values for each sample. Better with Sampler::round().
  void request_2D_array(int n);
  /// @brief Get the requested array of sample vectors.
  /// @param n Number of samples for verification.
  /// @return Pointer to the first sample.
  const Point2f *get_2D_array(int n);
  /// @brief Get the best number of samples over given number.
  virtual int round(int n) { return n; }
  /// @brief Start next sample.
  /// @return true until the spp is exceeded.
  virtual bool next_sample();
  /// @brief Clone a sampler with the same strategy but different random seed.
  virtual std::unique_ptr<Sampler> clone(int seed) const = 0;
  /// @brief Set the index of sample for current pixel to generate NEXT sample.
  /// @return false if the spp is exceeded.
  virtual bool set_sample_index(int64_t idx);

  const int64_t m_spp;

 protected:
  std::vector<int> m_1D_array_sizes, m_2D_array_sizes;
  std::vector<std::vector<Float>> m_sample_1D_array;
  std::vector<std::vector<Point2f>> m_sample_2D_array;
  Point2i m_current_pixel;
  int64_t m_idx_current_pixel_sample;

 private:
  /// @brief Offset for both array size and corresponding array.
  size_t m_1D_array_offset, m_2D_array_offset;
};

/**
 * @brief Samplers that generates definite numbers of samples.
 *        Number of dimensions will not be known until the ray goes into the
 *        scene. Uniform random values will be returned if number of dimensions
 *        is exceeded.
 */
class PixelSampler : public Sampler {
 public:
  PixelSampler(int64_t samples_per_pxiel, int sample_dims);
  bool next_sample() override;
  bool set_sample_index(int64_t idx) override;
  Float sample_1D() override;
  Point2f sample_2D() override;

 protected:
  // Sample values for MULTIPLE samples in ONE pixel.
  // Layout: m_samplexD[dim_idx][sample_idx].
  std::vector<std::vector<Float>> m_sample_1D;
  std::vector<std::vector<Point2f>> m_sample_2D;
  // Indexing the corresponding array in m_sample_[12]D.
  int m_idx_current_1D = 0, m_idx_current_2D = 0;
  RNG m_rng;
};

/**
 * @brief GlobalSampler is to convert image-range sequences into pixel-by-pixel
 *        range, so that the pixel-tile based multi-therading will be easy.
 */
class GlobalSampler : public Sampler {
 public:
  GlobalSampler(int64_t samples_per_pixel);
  /// @brief Get the global index of sample vector based on current pixel and
  ///        sample index in this pixel.
  virtual int64_t global_index(int64_t native_index) const = 0;
  /// @brief Get the component value of sample vector of given global index and
  ///        dimension index.
  /// @param global_idx_sample Global index of sample. Start from 0.
  /// @param idx_dim Index of dimension. Start from 0.
  /// @return Sample value. Scaled by resolution to offset into pixel for
  ///         first 2 dimension.
  virtual Float value_by_dimension(int64_t global_idx_sample,
                                   int idx_dim) const = 0;
  void start_pixel(const Point2i &p) override;
  bool next_sample() override;
  bool set_sample_index(int64_t idx) override;
  Float sample_1D() override;
  Point2f sample_2D() override;

 private:
  // Sampler will generate a sample value for this dimension.
  int m_dimension;
  int64_t m_global_idx_current_sample;
  // GlobalSampler fill values by dimension.
  // The first some dimensions of sequence are saved for regular 1D and 2D
  // samples. Subsequent dimensions are for array samples.
  static const int m_idx_array_start_dim = 5;
  // Finite dimensions.
  int m_idx_array_end_dim;
};
}  // namespace TRay