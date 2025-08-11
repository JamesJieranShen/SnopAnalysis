#pragma once

#include <format>
#include <iostream>
#include <map>
#include <string>

namespace SnopAnalysis {

class Logger {
public:
  enum class Verbosity { Silent = -1, Error = 0, Warn = 1, Info = 2, Debug = 3, Trace = 4 };
  inline static std::map<std::string, Verbosity> VerbosityMap = {
      {"silent", Verbosity::Silent}, {"error", Verbosity::Error}, {"warn", Verbosity::Warn},
      {"info", Verbosity::Info},     {"debug", Verbosity::Debug}, {"trace", Verbosity::Trace},
  };
  static void SetLevel(Verbosity level) { sLevel = level; }

  static void Die(const std::string& msg) {
    std::cerr << std::format("{}[FATAL]   {}{}\n", Color::Red, msg, Color::Reset);
    std::exit(EXIT_FAILURE);
  }

  static void Error(const std::string& msg) {
    if (sLevel >= Verbosity::Error) {
      std::cerr << std::format("{}[ERROR]   {}{}\n", Color::Red, msg, Color::Reset);
    }
  }

  static void Warn(const std::string& msg) {
    if (sLevel >= Verbosity::Warn) {
      std::cerr << std::format("{}[WARNING] {}{}\n", Color::Yellow, msg, Color::Reset);
    }
  }

  static void Info(const std::string& msg) {
    if (sLevel >= Verbosity::Info) {
      std::cerr << std::format("{}[INFO]    {}{}\n", Color::Green, msg, Color::Reset);
    }
  }

  static void Debug(const std::string& msg) {
    if (sLevel >= Verbosity::Debug) {
      std::cerr << std::format("{}[DEBUG]   {}{}\n", Color::Blue, msg, Color::Reset);
    }
  }

  static void Trace(const std::string& msg) {
    if (sLevel >= Verbosity::Trace) {
      std::cerr << std::format("{}[TRACE]   {}{}\n", Color::Gray, msg, Color::Reset);
    }
  }

private:
  static inline Verbosity sLevel = Verbosity::Info;

  struct Color {
    static constexpr std::string_view Reset = "\033[0m";
    static constexpr std::string_view Red = "\033[31m";
    static constexpr std::string_view Magenta = "\033[35m";
    static constexpr std::string_view Yellow = "\033[33m";
    static constexpr std::string_view Cyan = "\033[96m";
    static constexpr std::string_view Green = "\033[32m";
    static constexpr std::string_view Blue = "\033[34m";
    static constexpr std::string_view Gray = "\033[90m";
  };
};
} // namespace SnopAnalysis
