//
// Created by thm on 6/9/20.
//
// The word 'nothing' is a palindrome. If you spell it backwards is says 'gnihton' which also means nothing.
#pragma once
#include <filesystem>
#include <fmt/core.h>
#include <iostream>
#include <regex>
#include <string>
#include <syslog.h>
#include <type_traits>

namespace rov {
  class logSettings {
public:
        enum outputs {
      SYSLOG = 1,
      STD_OUT = 2,
      STD_ERR = 4
    };

    static inline int logLevel = LOG_DEBUG;// Uses syslog defines
    static inline void openValves(int enable) {
      valves = static_cast<outputs>(enable);
    }
    static inline bool isValveOpen(outputs enable) {
      return logSettings::valves & enable;
    }

private:
    static inline outputs valves = STD_OUT;
  };

  namespace private_do_not_use {
    template<typename T>
    struct is_string : public std::disjunction<std::is_same<char *, typename std::decay<T>::type>, std::is_same<const char *, typename std::decay<T>::type>,
                                               std::is_same<std::string, typename std::decay<T>::type>> {};

    template<typename T>
    constexpr void out(int priority, const T &fs) {

      if (logSettings::isValveOpen(logSettings::outputs::STD_OUT)) {
        std::cout << fs;
      }
      if (logSettings::isValveOpen(logSettings::outputs::STD_ERR)) {
        std::cerr << fs;
      }
      if (logSettings::isValveOpen(logSettings::outputs::SYSLOG)) {
        syslog(priority, "%s", fs.c_str());
      }
    }

    template<typename argType>
    constexpr auto format(int priority,
                          const std::string_view &file,
                          const char *function,
                          int32_t line,
                          const char *argName,
                          argType argValue) {
      if (priority > logSettings::logLevel) {
        return (argValue);
      }

      if (is_string<argType>::value) {
        out(priority, fmt::format("{}::{}({}): {}\n", file, function, line, argValue));
      } else {
        out(priority, fmt::format("{}::{}({}): <{}>-->[{}]\n", file, function, line, argName, argValue));
      }
      return (argValue);
    }
    // TODO: find a way to unify the formatting
    template<typename argType>
    constexpr auto formatHex(int priority,
                             const std::string_view &file,
                             const char *function,
                             int32_t line,
                             const char *argName,
                             argType argValue) {
      if (priority > logSettings::logLevel) {
        return (argValue);
      }

      if (is_string<argType>::value) {
        out(priority, fmt::format("{}::{}({}): {}\n", file, function, line, argValue));
      } else {
        out(priority, fmt::format("{}::{}({}): <{}>-->[{:#04x}]\n", file, function, line, argName, argValue));
      }
      return (argValue);
    }

    template<typename argType>
    constexpr auto formatArray(int priority,
                               const std::string_view &file,
                               const char *function,
                               int32_t line,
                               const char *argName,
                               argType argValue,
                               size_t size) {
      if (priority > logSettings::logLevel) {
        return (argValue);
      }

      out(priority, fmt::format("=== {}::{}({}) START ===\n", file, function, line));
      for (size_t i = 0; i < size; ++i) {
        out(priority, fmt::format(" {}[{}] --> [{:#04x}]\n", argName, i, static_cast<uint64_t>((argValue[i]))));
      }
      out(priority, fmt::format("=== {}::{}({}) End ===\n", file, function, line));
      return (argValue);
    }

  }// namespace private_do_not_use


#define LOG(priority, ...) private_do_not_use::format((priority), std::filesystem::path(__FILE__).filename().string(), __FUNCTION__, __LINE__, #__VA_ARGS__, __VA_ARGS__)
#define LOG_HEX(priority, ...) private_do_not_use::formatHex((priority), std::filesystem::path(__FILE__).filename().string(), __FUNCTION__, __LINE__, #__VA_ARGS__, __VA_ARGS__)
#define LOG_ARR(priority, arr, size) private_do_not_use::formatArray((priority), std::filesystem::path(__FILE__).filename().string(), __FUNCTION__, __LINE__, #arr, arr, size)

#define DBP(...) LOG(LOG_DEBUG, __VA_ARGS__)
#define DBP_HEX(...) LOG_HEX(LOG_DEBUG, __VA_ARGS__)
#define DBP_ARR(arr, size) LOG_ARR(LOG_DEBUG, arr, size)

  struct color {
    static std::string white(const std::string &str) { return "\e[30m" + str + "\e[0m"; }
    static std::string black(const std::string &str) { return "\e[30m" + str + "\e[0m"; }
    static std::string red(const std::string &str) { return "\e[31m" + str + "\e[0m"; }
    static std::string green(const std::string &str) { return "\e[32m" + str + "\e[0m"; }
    static std::string greenLight(const std::string &str) { return "\e[92m" + str + "\e[0m"; }
    static std::string brown(const std::string &str) { return "\e[33m" + str + "\e[0m"; }
    static std::string blue(const std::string &str) { return "\e[34m" + str + "\e[0m"; }
    static std::string magenta(const std::string &str) { return "\e[35m" + str + "\e[0m"; }
    static std::string cyan(const std::string &str) { return "\e[36m" + str + "\e[0m"; }
    static std::string gray(const std::string &str) { return "\e[37m" + str + "\e[0m"; }
    static std::string yellow(const std::string &str) { return "\e[43m" + str + "\e[0m"; }

    static std::string bg_black(const std::string &str) { return "\e[40m" + str + "\e[0m"; }
    static std::string bg_red(const std::string &str) { return "\e[41m" + str + "\e[0m"; }
    static std::string bg_green(const std::string &str) { return "\e[42m" + str + "\e[0m"; }
    static std::string bg_yell(const std::string &str) { return "\e[103m" + str + "\e[0m"; }
    static std::string bg_brown(const std::string &str) { return "\e[43m" + str + "\e[0m"; }
    static std::string bg_blue(const std::string &str) { return "\e[44m" + str + "\e[0m"; }
    static std::string bg_magenta(const std::string &str) { return "\e[45m" + str + "\e[0m"; }
    static std::string bg_cyan(const std::string &str) { return "\e[46m" + str + "\e[0m"; }
    static std::string bg_gray(const std::string &str) { return "\e[47m" + str + "\e[0m"; }

    static std::string bold(const std::string &str) { return "\e[1m" + str + "\e[0m"; }
    static std::string dim(const std::string &str) { return "\e[2m" + str + "\e[22m"; }
    static std::string italic(const std::string &str) { return "\e[3m" + str + "\e[23m"; }
    static std::string underline(const std::string &str) { return "\e[4m" + str + "\e[24m"; }
    static std::string blink(const std::string &str) { return "\e[5m" + str + "\e[25m"; }
    static std::string reverse_color(const std::string &str) { return "\e[7m" + str + "\e[27m"; }

    static std::string removeColor(const std::string &str) { return std::regex_replace(str, std::regex("(\e\\[\\d*m)"), ""); }
  };
}// namespace rov

#define MARKER rov::DBP("\n\n@@@@@@!!!MARKER!!!--------!!!MARKER!!!@@@@@@\n\n");
using rDye = rov::color;
