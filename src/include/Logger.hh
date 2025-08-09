#pragma once

#include <format>
#include <iostream>
#include <string>

namespace SnopAnalysis {
enum class Verbosity { Silent = 0, Warning = 1, Info = 2, Debug = 3 };

class Logger {
public:
  static void SetLevel(Verbosity level) { sLevel = level; }

  static void Warning(const std::string &msg) {
    if (sLevel >= Verbosity::Warning) {
      std::cerr << std::format("{}[WARNING] {}{}\n", Color::Yellow, msg,
                               Color::Reset);
    }
  }

  static void Info(const std::string &msg) {
    if (sLevel >= Verbosity::Info) {
      std::cerr << std::format("{}[INFO]    {}{}\n", Color::Cyan, msg,
                               Color::Reset);
    }
  }

  static void Debug(const std::string &msg) {
    if (sLevel >= Verbosity::Debug) {
      std::cerr << std::format("{}[DEBUG]   {}{}\n", Color::Gray, msg,
                               Color::Reset);
    }
  }

private:
  static inline Verbosity sLevel = Verbosity::Info;

  struct Color {
    static constexpr std::string_view Reset = "\033[0m";
    static constexpr std::string_view Yellow = "\033[33m";
    static constexpr std::string_view Cyan = "\033[36m";
    static constexpr std::string_view Gray = "\033[90m";
  };
};
} // namespace SnopAnalysis
