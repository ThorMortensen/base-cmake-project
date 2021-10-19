//
// Created by thm on 9/30/21.
//

#include "timeConductor.h"
#include <chrono>
#include <ppsMonitor.h>
#include <rovtime.h>
#include <thread>

namespace rov {

  timeConductor::timeConductor(baton_t *baton, const uint32_t msApproach, const uint32_t msTimeout, uint8_t phase) : baton(baton),
                                                                                                                     approachLockTime_ms(rov::time::MILLISECONDS - msApproach),
                                                                                                                     timeout_ms(msTimeout * MS_TO_NS),
                                                                                                                     phase(phase) {}

  void timeConductor::start() {
    using namespace std::chrono_literals;

    rov::ppsMonitor pps(phase);
    struct timespec timeout {
      .tv_sec = 0, .tv_nsec = static_cast<__syscall_slong_t>(timeout_ms)
    };
    uint64_t ppsTime = 0;

    while (run) {
      try {
        std::unique_lock<std::mutex> lck(baton->mtx);
        baton->miss = !pps.waitForPPSTimestamp(&ppsTime, timeout);
        if (baton->miss) {
          baton->timeStamp += rov::time::MICROSECONDS;
        } else {
          baton->timeStamp = ppsTime;
        }
        baton->stale = false;
      } catch (const std::exception &e) {
        rov::LOG(LOG_ERR,rDye::red(e.what()));
      }
      baton->cv.notify_all();
      std::this_thread::sleep_for(std::chrono::milliseconds(approachLockTime_ms));
    }
  }
  void timeConductor::stop() {
    run = false;
  }



  }// namespace rov
