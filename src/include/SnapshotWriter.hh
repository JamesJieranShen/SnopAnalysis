#pragma once
#include "OutputWriter.hh"
#include "nlohmann/json.hpp"

namespace SnopAnalysis {

class SnapshotWriter : public OutputWriter {
public:
  void Configure(const nlohmann::json &config) override;
  void Write(ROOT::RDF::RNode df) override;
  void WriteContext() const override;

private:
  std::string fTreeName;
  std::string fFileName;
  std::vector<std::string> fColumnNames;
  ROOT::RDF::RSnapshotOptions fOpts;
};
} // namespace SnopAnalysis
