//
// Created by thm on 25/06/2020.
//

#include <array>
#include <log.h>
#include <memory>
#include <ntpMonitor.h>
#include <string>
#include <unistd.h>


namespace rov {

  std::string exec(const char *cmd) {
    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
      throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
      result += buffer.data();
    }
    return result;
  }


  void ntpMonitor::start() {
    isNtpSync = false;
    run = true;

    while (run) {
      auto ntpstat = exec("ntpstat");
      ntpMonitor::isNtpSync = ntpstat.find("unsynchronised") == std::string::npos && ntpstat.find("synchronised") != std::string::npos;
      if (7 <= rov::logSettings::logLevel)
        private_do_not_use::format(
                (7), std::filesystem::path("_file_name_").filename().string(),
                "_function_name_", 36,
                "rDye::blue(\"NTP sync: \" + std::to_string(ntpMonitor::isNtpSync))",
                rDye::blue("NTP sync: " + std::to_string(ntpMonitor::isNtpSync)));
      sleep(10);
    }
  }

  void ntpMonitor::stop() {
    run = false;
  }

}// namespace rov
