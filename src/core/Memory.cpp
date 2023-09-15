#pragma once
#include "core/Memory.h"
#include <list>

namespace TRay {
void *alloc_aligned(size_t size) {
#ifdef TRAY_HAVE__ALIGNED_MALLOC
  return _aligned_malloc(size, TRAY_L1_CACHELINE_SIZE);
#else
  return memalign(TRAY_L1_CACHELINE_SIZE, size);
#endif
}
void free_aligned(void *ptr) {
  if (!ptr)
    return;
#ifdef TRAY_HAVE__ALIGNED_MALLOC
  _aligned_free(ptr);
#else
  free(ptr);
#endif
}

} // namespace TRay