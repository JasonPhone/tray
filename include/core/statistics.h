/// @file statistics.h
/// @author ja50n (zs_feng@qq.com)
/// @brief Statics and profiling.
/// @version 0.1
/// @date 2023-10-21
#include <functional>
#include <map>
#include <mutex>
#include <ostream>
#include <vector>

#include "TRay.h"
#include "core/stringformat.h"

/// @brief Statstics are stored in each thread temporarily, and
///        will finally get merged into a global accumulator.
///        The accumulator is supposed to be accessed only for
///        an existed thread-specific statistic variable.
///        We need to bridge the thread-scope variable and accumulator.
///        Actually just a way to pass the value to accumulator.
///        A callback method should be fine, a class which contains
///        a static callback list and static push-back method, with mutex.
///        It is singleton here.
///
namespace TRay {
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
}  // namespace TRay