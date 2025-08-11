#include "AliasStep.hh"

#include "StepRegistry.hh"

namespace SnopAnalysis {

void
AliasStep::Configure(const nlohmann::json& config) {
  Step::Configure(config);
  fName = config.at("name").get<std::string>();
  fAlias = config.at("alias").get<std::string>();
}

ROOT::RDF::RNode
AliasStep::DoExecute(ROOT::RDF::RNode input) {
  return input.Alias(fAlias, fName);
}

REGISTER_STEP("alias", AliasStep);
} // namespace SnopAnalysis
