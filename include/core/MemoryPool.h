/**
 * @file Memory.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Memory management utils.
 * @version 0.1
 * @date 2023-09-13
 */
#pragma once
#include <stddef.h>

#include <list>

#include "core/TRay.h"

namespace TRay {
void *allocAligned(size_t size);
void freeAligned(void *ptr);
template <typename T>
T *allocAligned(size_t count) {
  return (T *)allocAligned(count * sizeof(T));
}

class
#ifdef TRAY_HAVE_ALIGNAS
    alignas(TRAY_L1_CACHELINE_SIZE)
#endif
        MemoryPool {
 public:
  MemoryPool(size_t block_size = 262144 /* 256KB */)
      : kMinBlockSize_(block_size) {}
  ~MemoryPool();
  void *alloc(size_t n_bytes);
  /// @brief Alloc a series of instances from memory pool.
  ///
  /// @param n Number of instances.
  /// @param run_cstr
  template <typename T>
  T *alloc(size_t n = 1, bool run_cstr = true) {
    T *ret = (T *)alloc(n * sizeof(T));
    if (run_cstr)
      for (size_t i = 0; i < n; i++)
        // Placement new.
        new (&ret[i]) T();
    return ret;
  }
  /// @brief Reset in-block offset and clear all used blocks into available.
  /// @note  Blocks are not released, but should not be accessed in case of
  ///        being given to other use by alloc().
  void reset() {
    current_block_pos_ = 0;
    available_blocks_.splice(available_blocks_.begin(), used_blocks_);
  }

 private:
  MemoryPool(const MemoryPool &) = delete;
  MemoryPool &operator=(const MemoryPool &) = delete;
  // Least size of the chuncks in Byte.
  const size_t kMinBlockSize_;

  size_t current_block_pos_ = 0, current_block_size_ = 0;
  uint8_t *current_block_ = nullptr;
  // Size of each block is not fixed, but at least block_size_.
  std::list<std::pair<size_t, uint8_t *>> used_blocks_, available_blocks_;
};

}  // namespace TRay