/**
 * @file parallel.h
 * @author ja50n (zs_feng@qq.com)
 * @brief TRay parallelism.
 * @version 0.1
 * @date 2024-01-17
 */

#include <functional>
#include <thread>

#include "TRay.h"

namespace TRay {
// Persistent threads.
static std::vector<std::thread> worker_threads;
static bool kill_threads = false;

int nSystemCores();
/**
 * @brief The parallelism. One thread per core. No guarantee of concurrent
 * safety.
 *
 * @param func void(int), the function to be executed.
 * @param count Number of loop iterations.
 * @param iter_per_thread Number of loop iterations per thread.
 */
void parallelFor(const std::function<void(int)> &func, int count,
                 int iter_per_thread) {
  if (TRAY_N_THREADS == 1 || count < iter_per_thread) {
    // Run immediately.
    for (int i = 0; i < count; i++) func(i);
    return;
  }
  if (worker_threads.empty()) {
    for (int i = 0; i < nSystemCores(); i++)
      ;
  }
}
int nSystemCores() { return std::max(1u, std::thread::hardware_concurrency()); }
}  // namespace TRay
