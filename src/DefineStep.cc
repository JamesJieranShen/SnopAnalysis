#include "DefineStep.hh"

#include "StepRegistry.hh"

namespace SnopAnalysis {

void
DefineStep::Configure(const nlohmann::json& config) {
  fName = config.at("name").get<std::string>();
  fExpression = config.at("expr").get<std::string>();
}

ROOT::RDF::RNode
DefineStep::Execute(ROOT::RDF::RNode input) {
  return input.Define(fName, fExpression);
}

REGISTER_STEP("define", DefineStep);
} // namespace SnopAnalysis
