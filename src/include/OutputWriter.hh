#pragma once
#include <Logger.hh>
#include <ROOT/RDataFrame.hxx>
#include <nlohmann/json.hpp>

namespace SnopAnalysis {
class OutputWriter {
public:
  virtual ~OutputWriter() = default;
  virtual void Configure(const nlohmann::json &config) = 0;

  virtual void Write(ROOT::RDF::RNode df) = 0;
  virtual void WriteConfig([[maybe_unused]] nlohmann::json &config) {
    Logger::Warning(
        "WriteConfig called on a OutputWriter that doesn't support it!");
  }
};
} // namespace SnopAnalysis
