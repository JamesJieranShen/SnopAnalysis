#include "ReportStep.hh"

#include "StepRegistry.hh"

namespace SnopAnalysis {

void
ReportStep::Configure(const nlohmann::json& config) {
  Step::Configure(config);
}

ROOT::RDF::RNode
ReportStep::DoExecute(ROOT::RDF::RNode input) {
  fReport = input.Report();
  return input;
}

void
ReportStep::DoReport() {
  std::cout << "Filter Efficiency Reports:" << std::endl;
  fReport->Print();
}

REGISTER_STEP("report", ReportStep);
} // namespace SnopAnalysis
