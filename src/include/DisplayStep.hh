#pragma once
#include "Step.hh"

namespace SnopAnalysis {
class DisplayStep : public Step {
public:
  void Configure(const nlohmann::json& config) override;
  ROOT::RDF::RNode DoExecute(ROOT::RDF::RNode input) override;
};

} // namespace SnopAnalysis
