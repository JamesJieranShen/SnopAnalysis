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
  if (fContext->fReports.contains("efficiencies")) {
    Logger::Warn("Efficiency report 'efficiencies' already exists. It will be overwritten.");
  }
  nlohmann::json reportJson;
  const auto all_entries = fReport->end() == fReport->begin() ? 0ULL : fReport->begin()->GetAll();
  for (const auto& cutInfo : *fReport) {
    nlohmann::json cutJson;
    cutJson["name"] = cutInfo.GetName();
    cutJson["all"] = cutInfo.GetAll();
    cutJson["pass"] = cutInfo.GetPass();
    cutJson["efficiency"] = cutInfo.GetEff();
    cutJson["cumulative_efficiency"] = float(cutInfo.GetPass()) / float(all_entries);
    reportJson.push_back(cutJson);
  }
  fContext->fReports["efficiencies"] = reportJson;
}

REGISTER_STEP("report", ReportStep);
} // namespace SnopAnalysis
