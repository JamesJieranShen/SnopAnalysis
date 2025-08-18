#include "DescribeStep.hh"

#include "StepRegistry.hh"

namespace SnopAnalysis {

void
DescribeStep::Configure(const nlohmann::json& config) {
  Step::Configure(config);
  fShortFormat = config.value("short_format", false);
}

ROOT::RDF::RNode
DescribeStep::DoExecute(ROOT::RDF::RNode input) {
  Logger::Info("================================================================================");
  Logger::Info(input.Describe().AsString(fShortFormat));
  Logger::Info("================================================================================");
  return input;
}

REGISTER_STEP("describe", DescribeStep);
} // namespace SnopAnalysis
