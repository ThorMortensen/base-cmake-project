//
// Created by thm on 25/06/2020.
//
#pragma once


#include <atomic>
#include <chrono>

namespace rov {

  class ntpMonitor {
public:
    void start();
    inline static std::atomic_bool isNtpSync = true;
    void stop();

private:
    std::atomic_bool run;
    static constexpr auto noTime = std::chrono::system_clock::time_point();
  };

}// namespace rov
