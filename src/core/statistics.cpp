/// @file statistics.cpp
/// @author ja50n (zs_feng@qq.com)
/// @brief Impls.
/// @version 0.1
/// @date 2023-10-22

#include "core/statistics.h"

#include "core/stringformat.h"

namespace TRay {
// Supposed to be singleton.
static StatsAccumulator statsAccumulator;

std::vector<std::function<void(StatsAccumulator &)>>
    *StatReporter::m_callbacks = nullptr;
void StatReporter::CallCallbacks(StatsAccumulator &accum) {
  for (auto cb : *m_callbacks) cb(accum);
}

void ReportThreadStats() {
  static std::mutex cb_mutex;
  std::lock_guard<std::mutex> lock(cb_mutex);
  StatReporter::CallCallbacks(statsAccumulator);
}
void PrintStats(std::ostream &o) { statsAccumulator.Print(o); }
void ClearStats() { statsAccumulator.Clear(); }

static void SplitCategory(const std::string &category, std::string *main_cat,
                          std::string *sub_cat) {
  size_t idx = category.find_first_of('/');
  if (idx != std::string::npos) {
    *main_cat = category.substr(0, idx);
    *sub_cat = category.substr(idx + 1);
  } else {
    *sub_cat = category;
  }
}

void StatsAccumulator::Print(std::ostream &o) {
  o << "Statistics:\n";
  std::map<std::string, std::vector<std::string>> print_buffer;
  // Counter.
  for (auto counter : m_counters) {
    std::string main_cat, sub_cat;
    SplitCategory(counter.first, &main_cat, &sub_cat);
    if (main_cat.empty()) main_cat = "Single category:";
    print_buffer[main_cat].push_back(
        string_format("%-16s: %lld", sub_cat.c_str(), counter.second));
  }
  // Main print.
  for (auto category : print_buffer) {
    o << string_format("  %s\n", category.first.c_str());
    for (auto entry : category.second)
      o << string_format("    %s\n", entry.c_str());
  }
}
void StatsAccumulator::Clear() {
  ///
  m_counters.clear();
}

// For tests.
// STAT_COUNTER("main/sub", cnt)
// void TestFunc() {
//   for (int i = 0; i < 100000; i++) cnt++;
//   std::cout << "thread done " << cnt << std::endl;
//   TRay::ReportThreadStats();
// }

/**
 * @note  The profiling method used in pbrt-v3 is unavailable on Windows.
 *        Profiling function is postponed.
 */
// struct ProfileSample {
//   std::atomic<prof_state_bit> sample_state{0};
//   std::atomic<std::uint64_t> sample_count{0};
// };
// // We use a hash table to keep track of the profiler state counts. Because
// // we can't do dynamic memory allocation in a signal handler (and because
// // the counts are updated in a signal handler), we can't easily use
// // std::unordered_map.  We therefore allocate a fixed size hash table and
// // use linear probing if there's a conflict.
// static constexpr int kProfileHashSize = 256;
// static std::array<ProfileSample, kProfileHashSize> profile_samples;
// static std::chrono::system_clock::time_point profile_start_time;

// thread_local prof_state_bit profiler_state;
// static std::atomic_bool profiler_running{false};
// void initProfiler() {
//   if (profiler_running) SError("Profiler is already running!!!");
//   profiler_state = profileStateBit(Prof::SceneConstruction);
//   clearProfiler();
//   profile_start_time = std::chrono::system_clock::now();

//   struct sigaction sa;

// }

// void suspendProfiler();
// void resumeProfiler();
// void profilerWorkerThreadInit();
// void reportProfilerResults(FILE *dest);
// void clearProfiler();
// void cleanupProfiler();
}  // namespace TRay