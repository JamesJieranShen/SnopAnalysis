#include "DisplayStep.hh"

#include "StepRegistry.hh"

namespace SnopAnalysis {

void
DisplayStep::Configure(const nlohmann::json& config) {
  Step::Configure(config);
}

ROOT::RDF::RNode
DisplayStep::DoExecute(ROOT::RDF::RNode input) {
  std::cout << "================================================================================" << std::endl;
  input.Describe().Print();
  std::cout << "================================================================================" << std::endl;
  return input;
}

REGISTER_STEP("display", DisplayStep);
} // namespace SnopAnalysis
