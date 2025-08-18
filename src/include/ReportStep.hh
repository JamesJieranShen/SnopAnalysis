#pragma once
#include "Step.hh"

namespace SnopAnalysis {
class ReportStep : public Step {
public:
  void Configure(const nlohmann::json& config) override;
  ROOT::RDF::RNode DoExecute(ROOT::RDF::RNode input) override;
  void DoReport() override;

private:
  ROOT::RDF::RResultPtr<ROOT::RDF::RCutFlowReport> fReport;
};

} // namespace SnopAnalysis
