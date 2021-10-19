//
// Created by thm on 9/24/21.
//

#include <errno.h>
#include <fcntl.h>
#include <log.h>
#include <ppsMonitor.h>

#include <cstring>
#include <rovtime.h>

namespace rov {

  ppsMonitor::ppsMonitor(uint8_t ppsPhase, std::string path) : ppsPhase(ppsPhase), path(std::move(path)) {
    find_source();
  }

  /**
 * Gets the PPS rising edge time from the Linux PPS driver.
 * @returns uint64_t us epoch time-stamp.
 */
  bool ppsMonitor::waitForPPSTimestamp(uint64_t *timeStamp, struct timespec timeout) {

    pps_info_t infobuf;

  retry:
    int ret = time_pps_fetch(handle, PPS_TSFMT_TSPEC, &infobuf, &timeout);

    if (ret < 0) {
      if (ret == -EINTR) {
        rov::LOG(LOG_WARNING,rDye::red("time_pps_fetch() got a signal!. Retry!"));
        goto retry;
      } else if (errno == ETIMEDOUT) {
        return false;
      }
      throw std::system_error(errno, std::generic_category(), strerror(errno));
    }

    if (ppsPhase == 0) {
      *timeStamp = rov::time::timespecToUs(infobuf.assert_timestamp);
      return true;
    }

    *timeStamp = rov::time::timespecToUs(infobuf.clear_timestamp);
    return true;
  }
  /**
 * Establishes a connection to the system PPS driver.
 */
  void ppsMonitor::find_source() {
    pps_params_t params;
    int ret;

    /* Try to find the source by using the supplied "path" name */
    ret = open(path.c_str(), O_RDWR);

    if (ret < 0) {
      throw std::system_error(errno, std::generic_category(), "Unable to open device. Is the PPS driver enabled?");
    }

    /* Open the PPS source (and check the file descriptor) */
    ret = time_pps_create(ret, &handle);
    if (ret < 0) {
      throw std::system_error(errno, std::generic_category(), "cannot create a PPS source from device ");
    }

    /* Find out what features are supported */
    ret = time_pps_getcap(handle, &avail_mode);
    if (ret < 0) {
      throw std::system_error(errno, std::generic_category(), "cannot get capabilities ");
    }

    if ((avail_mode & PPS_CANWAIT) == 0) {
      throw std::system_error(errno, std::generic_category(), "cannot PPS_CANWAIT: PPS device doesn't support wait");
    }

    if ((avail_mode & PPS_CAPTUREASSERT) == 0) {
      throw std::system_error(errno, std::generic_category(), "cannot CAPTUREASSERT");
    }

    /* Capture assert timestamps */
    ret = time_pps_getparams(handle, &params);
    if (ret < 0) {
      throw std::system_error(errno, std::generic_category(), "cannot get parameters");
    }
    params.mode |= PPS_CAPTUREASSERT;
    /* Override any previous offset if possible */
    if ((avail_mode & PPS_OFFSETASSERT) != 0) {
      params.mode |= PPS_OFFSETASSERT;
      params.assert_offset = offset_assert;
    }

    ret = time_pps_setparams(handle, &params);
    if (ret < 0) {
      throw std::system_error(errno, std::generic_category(), "cannot set parameters");
    }
    rov::DBP(rDye::green("PPS Source found!"));
  }

  ppsMonitor::~ppsMonitor() {
    close(handle);
  }

  ppsMonitor::ppsMonitor(const ppsMonitor &other) {
  }


}// namespace rov