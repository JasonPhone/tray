#include "core/MemoryPool.h"

#include <list>

#include "core/statistics.h"

namespace TRay {
STAT_COUNTER("MemoryPool/aligned_alloc_call", alloc_counter);
STAT_COUNTER("MemoryPool/aligned_alloc_byte", alloc_size);
void *allocAligned(size_t size) {
  alloc_counter++;
  alloc_size += size;
#ifdef TRAY_HAVE__ALIGNED_MALLOC
  return _aligned_malloc(size, TRAY_L1_CACHELINE_SIZE);
#else
  return memalign(TRAY_L1_CACHELINE_SIZE, size);
#endif
}
void freeAligned(void *ptr) {
  if (!ptr) return;
#ifdef TRAY_HAVE__ALIGNED_MALLOC
  _aligned_free(ptr);
#else
  free(ptr);
#endif
}

MemoryPool::~MemoryPool() {
  freeAligned(current_block_);
  for (auto &block : used_blocks_) freeAligned(block.second);
  for (auto &block : available_blocks_) freeAligned(block.second);
  ReportThreadStats();
}
void *MemoryPool::alloc(size_t n_bytes) {
  // Round up to alignment, 16 Byte always.
  n_bytes = ((n_bytes + 15) & (~15));
  // See if a new block is needed.
  if (current_block_pos_ + n_bytes >= current_block_size_) {
    // Mark this block as used.
    if (current_block_) {
      used_blocks_.push_back({current_block_size_, current_block_});
      current_block_ = nullptr;
    }
    // Try available_blocks_.
    for (auto iter = available_blocks_.begin(); iter != available_blocks_.end();
         iter++) {
      if (iter->first >= n_bytes) {
        current_block_size_ = iter->first;
        current_block_ = iter->second;
        available_blocks_.erase(iter);
        break;
      }
    }
    // Then alloc new.
    if (!current_block_) {
      current_block_size_ = std::max(n_bytes, kMinBlockSize_);
      current_block_ = allocAligned<uint8_t>(current_block_size_);
    }
    current_block_pos_ = 0;
  }
  // Alloc and return.
  void *ret = current_block_ + current_block_pos_;
  current_block_pos_ += n_bytes;
  return ret;
}
}  // namespace TRay