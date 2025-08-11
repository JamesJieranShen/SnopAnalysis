#include "FilterStep.hh"

#include "StepRegistry.hh"

namespace SnopAnalysis {

void
FilterStep::Configure(const nlohmann::json& config) {
  Step::Configure(config);
  fExpression = config.at("expr").get<std::string>();
  fName = config.value("name", "");
}

ROOT::RDF::RNode
FilterStep::DoExecute(ROOT::RDF::RNode input) {
  return input.Filter(fExpression, fName);
}

REGISTER_STEP("filter", FilterStep);
} // namespace SnopAnalysis
