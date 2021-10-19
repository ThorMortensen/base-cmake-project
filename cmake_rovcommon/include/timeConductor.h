//
// Created by thm on 9/30/21.
//
#pragma once
#include "log.h"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>

namespace rov {
  /**
 * @brief
 * Time conductor will wait for PPS pulse and record the kernel time
 * of either the rising (phase=0) or falling (phase=1) edge
 * of the PPS signal. The time conductor will wake the thread
 * waiting for the PPS signal through the baton structure.
 *
 * The timeConductor will lock the baton while it waits for the PPS.
 * Please make sure the consumer thread is locked and waiting for
 * wakeup when the approach time is initiated.
 *
 * Here is an example on how to use the baton:
 *
 *     { // Wait for time conductors notify
 *       std::unique_lock<std::mutex> lck(baton.mtx); // Lock for wait
 *       while (baton.stale) { // Check for spurious wakeup
 *         baton.cv.wait(lck); // Wait for unlock
 *       }
 *       baton.stale = true; // Reset baton for next swing
 *       ppsTime = baton.timeStamp;
 *       if (baton.miss){ // Handle PPS miss
 *         rov::DBP(rDye::red("PPS MISS!!"));
 *       }
 *     }
 *
 * !!WARNING!! If the PPS generator misses a cycle pulse, the timeConductor will estimate the
 * expected time for the missed PPS. This estimation is quite rough
 * and the compounded error will quickly make the timestamp drift to an
 * unusable degree. The user must handle the miss event and decide if the
 * spoofed timestamp should be used or not.
 *
 *
 */

  class timeConductor {
public:
    typedef struct {
      std::mutex mtx;               // Access control
      std::condition_variable cv;   // Wait and notify
      uint64_t timeStamp = 0;       // The kernel time for the edge of the PPS
      std::atomic_bool stale = true;// Guard against spurious wakeup. User should reset this immediately after wakeup
      std::atomic_bool miss = true; // Indicate if PPS pulse was missed. In this case timestamp will be a estimate of when the PPS was expected.
    } baton_t;


    timeConductor(baton_t *baton, uint32_t msApproach = 100, uint32_t msTimeout = 300, uint8_t phase = 0);
    void start();
    void stop();

    static inline void initialPPSSync(timeConductor::baton_t *baton) {
      while (baton->miss) {
        std::unique_lock<std::mutex> lck(baton->mtx);
        while (baton->stale) {
          baton->cv.wait(lck);
        }
        baton->stale = true;
        rov::LOG(LOG_INFO, "Waiting for PPS sync...");
      }
    }

    static inline void waitForPPS(timeConductor::baton_t *baton) {
      std::unique_lock<std::mutex> lck(baton->mtx);// Lock for wait
      while (baton->stale) {                       // Check for spurious wakeup
        baton->cv.wait(lck);                       // Wait for unlock
      }
      baton->stale = true;// Reset baton for next swing
    }

private:
    static constexpr int32_t MS_TO_NS = 1000000;

    baton_t *baton;
    bool run = true;
    const uint8_t phase = 0;
    const uint32_t approachLockTime_ms = 0;
    const uint32_t timeout_ms = 0;
  };

}// namespace rov
