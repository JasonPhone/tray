/// @file statistics.h
/// @author ja50n (zs_feng@qq.com)
/// @brief Statics and profiling.
/// @version 0.1
/// @date 2023-10-21
///
#pragma once
#include <functional>
#include <map>
#include <mutex>
#include <ostream>
#include <vector>

#include "TRay.h"
#include "core/stringformat.h"

namespace TRay {
/// @brief Statistics are stored in each thread temporarily, and
///        will finally get merged into a global accumulator.
///        The accumulator is supposed to be accessed only for
///        an existed thread-specific statistic variable.
///        We need to bridge the thread-scope variable and accumulator.
///        Actually just a way to pass the value to accumulator.
///        A callback method should be fine, a class which contains
///        a static callback list and static push-back method, with mutex.
///        It is singleton here.
///
class StatsAccumulator;
class StatReporter {
 public:
  StatReporter(std::function<void(StatsAccumulator &)> cb_func) {
    static std::mutex cb_mutex;
    std::lock_guard<std::mutex> lock(cb_mutex);
    if (!m_callbacks)
      m_callbacks = new std::vector<std::function<void(StatsAccumulator &)>>;
    m_callbacks->push_back(cb_func);
  }
  friend void ReportThreadStats();

 private:
  static std::vector<std::function<void(StatsAccumulator &)>> *m_callbacks;
  static void CallCallbacks(StatsAccumulator &accum);
};

void ReportThreadStats();
// Will access the actual accumulator.
void PrintStats(std::ostream &o);
void ClearStats();

class StatsAccumulator {
 public:
  void AccumulateCounter(const std::string &cat, const int64_t val) {
    m_counters[cat] += val;
  }
  void Print(std::ostream &o);
  void Clear();

 private:
  std::map<std::string, int64_t> m_counters;
};

/// @brief Macros to use in each CPP FILE.
///        They create a thread_local var for temp storage
///        and a callback for stat submitting.
///
#define STAT_COUNTER(category, var)                         \
  static thread_local int64_t var;                          \
  static void STAT_CB##var(TRay::StatsAccumulator &accum) { \
    accum.AccumulateCounter(category, var);                 \
    var = 0;                                                \
  }                                                         \
  static TRay::StatReporter STAT_REPORTER##var(STAT_CB##var);

// void TestFunc();

/**
 * @brief Profiling uses system call to slice the time, interrupt the working
 * threads, then accumulate the time some phase lasts. Phases of the thread are
 * described with an `uint32_t`, each bit to indicate if one state is active.
 * They form a stack-like hierarchy, which is NOT defined by calling, but the
 * declaration order of phases below.
 * @note Nice timer is unavailable on Win, so profiling is currently
 * unavailable.
 */
// using prof_state_bit = std::uint32_t;
// enum class Prof {
//   SceneConstruction,
//   AccelConstruction,
//   IntegratorRender,
//   SamplerIntegratorLi,
//   DirectLighting,
//   AccelIntersect,
//   AccelIntersectP,
//   TriIntersect,
//   TriIntersectP,
//   ComputeScatteringFuncs,
//   GenerateCameraRay,
//   BrdfEvaluation,
//   NProfStates
// };

// extern thread_local prof_state_bit profiler_state;
// inline prof_state_bit currentProfilerState() { return profiler_state; }
// inline prof_state_bit profileStateBit(Prof p) { return (1ull << (int)p); }
// void initProfiler();
// void suspendProfiler();
// void resumeProfiler();
// void profilerWorkerThreadInit();
// void reportProfilerResults(FILE *dest);
// void clearProfiler();
// void cleanupProfiler();
// class ProfilePhase {
//  public:
//   ProfilePhase(Prof p) {
//     state_bit_ = (1 << (int)p);
//     // If is repeating call.
//     reset_ = (profiler_state & state_bit_) == 0;
//     profiler_state |= state_bit_;
//   }
//   ~ProfilePhase() {
//     if (reset_) profiler_state &= ~state_bit_;
//   }

//  private:
//   bool reset_;
//   prof_state_bit state_bit_;
// };
}  // namespace TRay