#include "DescribeStep.hh"

#include "StepRegistry.hh"

namespace SnopAnalysis {

void
DescribeStep::Configure(const nlohmann::json& config) {
  Step::Configure(config);
}

ROOT::RDF::RNode
DescribeStep::DoExecute(ROOT::RDF::RNode input) {
  std::cout << "================================================================================" << std::endl;
  input.Describe().Print();
  std::cout << "================================================================================" << std::endl;
  return input;
}

REGISTER_STEP("describe", DescribeStep);
} // namespace SnopAnalysis
