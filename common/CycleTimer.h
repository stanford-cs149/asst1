#ifndef SYRAH_CYCLE_TIMER_H_
#define SYRAH_CYCLE_TIMER_H_

#include <chrono>
#include <cstdint>

class CycleTimer {
 public:
  using SysClock = std::uint64_t;
  using Clock = std::chrono::steady_clock;
  using Seconds = std::chrono::duration<double>;

  // Returns seconds elapsed since the first call, using a monotonic clock.
  // Thread-safe since C++11 (static local initialization is thread-safe).
  static inline double currentSeconds() {
    static const Clock::time_point t0 = Clock::now();
    return Seconds{Clock::now() - t0}.count();
  }

  // Utility class: no instances.
  CycleTimer() = delete;
  ~CycleTimer() = delete;
  CycleTimer(const CycleTimer&) = delete;
  CycleTimer& operator=(const CycleTimer&) = delete;
};

#endif  // SYRAH_CYCLE_TIMER_H_