/**
 * @file Memory.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Memory management utils.
 * @version 0.1
 * @date 2023-09-13
 */
#pragma once
#include "core/TRay.h"
#include <list>
#include <stddef.h>

namespace TRay {
void *alloc_aligned(size_t size);
void free_aligned(void *ptr);
template <typename T> T *alloc_aligned(size_t count) {
  return (T *)alloc_aligned(count * sizeof(T));
}

class
#ifdef TRAY_HAVE_ALIGNAS
    alignas(TRAY_L1_CACHELINE_SIZE)
#endif
        MemoryPool {
public:
  MemoryPool(size_t block_size = 212644 /* 256KB */)
      : m_block_size(block_size) {}

private:
  MemoryPool(const MemoryPool &) = delete;
  MemoryPool &operator=(const MemoryPool &) = delete;
  // Size of the chuncks.
  const size_t m_block_size;

  size_t m_current_block_pos = 0, m_current_alloc_size = 0;
  uint8_t *m_current_block = nullptr;
};

} // namespace TRay