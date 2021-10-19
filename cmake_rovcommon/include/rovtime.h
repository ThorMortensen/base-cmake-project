//
// Created by thm on 03/05/2021.
//

#pragma once

#include "log.h"
#include "ntpMonitor.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <thread>


namespace rov {

  class time {

public:
    static constexpr int32_t MICROSECONDS = 1000000;
    static constexpr int32_t MILLISECONDS = 1000;
    static constexpr int32_t US_TO_MS = 1000;

    static inline uint64_t now() {
      return std::chrono::duration_cast<std::chrono::microseconds>(
                     std::chrono::high_resolution_clock::now().time_since_epoch())
              .count();
    };

    static inline uint64_t since(uint64_t then) {
      return now() - then;
    };

    static inline uint64_t until(uint64_t when) {
      return when - now();
    };

    static inline uint64_t stamp() {
      return ntpMon.isNtpSync ? now() : 0;
    };

    static inline std::string nowStr() {
      return std::to_string(now());
    };

    static inline uint64_t usToNextSec() {
      return 1000000 - (now() % 1000000);
    };

    static inline uint64_t usSinceLastSec() {
      return (now() % 1000000);
    };


    static inline uint64_t thisSecond(uint64_t timePoint) {
      return (timePoint / MICROSECONDS) * MICROSECONDS;
    };

    static inline uint32_t usFromSecond(uint64_t timePoint, int64_t offset) {
      int64_t dist = std::abs(offset - int64_t((timePoint) % MICROSECONDS));
      int64_t f = MICROSECONDS - dist;
      return std::abs(std::min(dist, f));
    }

    static inline uint32_t usFromSecond(uint64_t timePoint) {
      return usFromSecond(timePoint, 0);
    }

    static inline uint64_t nextSecond(uint64_t timePoint) {
      return thisSecond(timePoint) + MICROSECONDS;
    };

    static inline uint64_t nextSecond() {
      return nextSecond(now());
    };

    static inline std::string stampToHStr(uint64_t timePoint) {
      std::ostringstream oss;

      auto stdTime = std::chrono::system_clock::to_time_t(
              std::chrono::time_point<std::chrono::system_clock>((std::chrono::microseconds(timePoint))));

      oss << std::put_time(std::localtime(&stdTime), "%Y-%m-%d %H:%M:%S");
      oss << '.' << std::setfill('0') << std::setw(6) << timePoint % 1000000;

      return oss.str();
    }

    static inline void startNtpMonitor() {
      ntpMonThread = std::thread(&ntpMonitor::start, &ntpMon);
    }

    static inline void stopNtpMonitorThread() {
      ntpMon.stop();
      ntpMonThread.join();
    }

    static inline bool isNtpSync() {
      return rov::ntpMonitor::isNtpSync;
    }


    static constexpr uint64_t timespecToUs(timespec ts) {
      auto duration = std::chrono::seconds{ts.tv_sec} + std::chrono::nanoseconds{ts.tv_nsec};
      return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    }


private:
    static inline ntpMonitor ntpMon;
    static inline std::thread ntpMonThread;
  };


}// namespace rov