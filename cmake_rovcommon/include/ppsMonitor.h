//
// Created by thm on 9/24/21.
//

#ifndef SPW_SUPERVISOR_PPSMONITOR_H
#define SPW_SUPERVISOR_PPSMONITOR_H
#include <string>

#include "timepps.h"

namespace rov {
  class ppsMonitor {


public:
    ppsMonitor(uint8_t ppsPhase = 0, std::string path = "/dev/pps0");
    virtual ~ppsMonitor();
    ppsMonitor(const ppsMonitor &other);// II. copy constructor

    void getLastPPSTime();

    bool waitForPPSTimestamp(uint64_t *timeStamp, struct timespec timeout);

private:
    void find_source();
    inline static struct timespec offset_assert = {0, 0};

    uint8_t ppsPhase = 0;
    std::string path;
    pps_handle_t handle{};
    int avail_mode{};
  };

}// namespace rov


#endif//SPW_SUPERVISOR_PPSMONITOR_H
