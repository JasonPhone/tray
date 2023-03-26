#include "core/TRay.h"
#include "core/geometry/Point.h"
#include "core/Camera.h"

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
}  // namespace TRay