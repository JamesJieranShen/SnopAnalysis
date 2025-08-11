#include "DisplayStep.hh"

#include "StepRegistry.hh"

namespace SnopAnalysis {

void
DisplayStep::Configure(const nlohmann::json& config) {
  Step::Configure(config);
}

ROOT::RDF::RNode
DisplayStep::DoExecute(ROOT::RDF::RNode input) {
  for (auto& col : input.GetColumnNames()) {
    std::cout << col << " : " << input.GetColumnType(col) << "\n";
  }
  return input;
}

REGISTER_STEP("display", DisplayStep);
} // namespace SnopAnalysis
