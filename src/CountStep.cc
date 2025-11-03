#include "CountStep.hh"

#include "StepRegistry.hh"

namespace SnopAnalysis {

void
CountStep::Configure(const nlohmann::json& config) {
  Step::Configure(config);
  fName = config.value("name", "");
  fCountTotal = config.value("count_total", true);
  fCountMC = config.value("count_mc", true);
  fTotalCountName = fName.empty() ? "total_count" : fName + "_total_count";
  fMCCountName = fName.empty() ? "mc_count" : fName + "_mc_count";
  nlohmann::json& report = fContext->fReports;
  if (fCountTotal && report.contains(fTotalCountName)) {
    Logger::Warn("Total count report '{}' already exists. It will be overwritten.", fTotalCountName);
  }
  if (fCountMC && report.contains(fMCCountName)) {
    Logger::Warn("MC count report '{}' already exists. It will be overwritten.", fMCCountName);
  }
}

ROOT::RDF::RNode
CountStep::DoExecute(ROOT::RDF::RNode input) {
  if (fCountTotal) {
    fTotalCountResult = input.Count();
  }
  if (fCountMC) {
    auto df =
        input.Define("mc_unique", [](Int_t mc_index, Int_t run_id) -> ULong64_t { return run_id * 1e9 + mc_index; },
                     {"mcIndex", "runID"});
    std::vector<ULong64_t> mcIndices = df.Take<ULong64_t>("mc_unique").GetValue();
    std::sort(mcIndices.begin(), mcIndices.end());
    auto last = std::unique(mcIndices.begin(), mcIndices.end());
    size_t mcCount = std::distance(mcIndices.begin(), last);
    fContext->fReports[fMCCountName] = mcCount;
  }
  return input;
}

void
CountStep::DoReport() {
  if (fCountTotal) fContext->fReports[fTotalCountName] = *fTotalCountResult;
}

REGISTER_STEP("count", CountStep);
} // namespace SnopAnalysis
