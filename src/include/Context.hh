#pragma once

#include "Config.hh"

#include <chrono>
#include <nlohmann/json.hpp>

namespace SnopAnalysis {

/**
 * Keeps track of the entire run context. Given a Context, one should be able to completely reproduce everything about
 * how a file is produced.
 * */
struct Context {
  std::shared_ptr<const nlohmann::json> fConfig;
  std::string fVersion;
  std::string fCmdLine;
  std::string fStartTime;
  size_t mt_threads = 1;
};

inline void
to_json(nlohmann::json& j, const Context& c) {
  j = nlohmann::json{{"config", *c.fConfig},
                     {"version", c.fVersion},
                     {"cmdline", c.fCmdLine},
                     {"startTime", c.fStartTime},
                     {"mt_threads", c.mt_threads}};
}

inline Context
MakeContext(const nlohmann::json& config, int argc, char** argv) {

  Context ctx;
  ctx.fConfig = std::make_shared<const nlohmann::json>(config);
  ctx.fVersion = SnopAnalysis::Build::VERSION;
  std::ostringstream cmd;
  for (int i = 0; i < argc; ++i) {
    cmd << argv[i] << " ";
  }
  ctx.fCmdLine = cmd.str();
  auto now = std::chrono::system_clock::now();
  std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
  std::ostringstream ts;
  ts << std::put_time(std::localtime(&now_time_t), "%FT%T%Z");
  ctx.fStartTime = ts.str();

  return ctx;
}

} // namespace SnopAnalysis
