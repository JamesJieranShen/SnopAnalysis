#pragma once
#include "Step.hh"

namespace SnopAnalysis {
class ResetStep : public Step {
public:
  void Configure(const nlohmann::json& config) override;
  ROOT::RDF::RNode DoExecute(ROOT::RDF::RNode input) override;

private:
  bool fPreserveMT = true;
  bool fMTOn;
};

} // namespace SnopAnalysis
