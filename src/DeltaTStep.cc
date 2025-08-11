#include "DeltaTStep.hh"

#include "Logger.hh"
#include "StepRegistry.hh"

#include <locale>

namespace SnopAnalysis {

void
DeltaTStep::Configure(const nlohmann::json& config) {
  Step::Configure(config);
  fName = config.value("name", "delta_t");
}

ROOT::RDF::RNode
DeltaTStep::DoExecute(ROOT::RDF::RNode input) {
  std::vector<ULong64_t> cc50 = input.Take<ULong64_t>("clockCount50").GetValue();
  Logger::Info(
      std::format(std::locale("en_US.UTF-8"), "Concretized a vector of size {:L} for clockCount50", cc50.size()));
  // override this in time with delta_t
  for (size_t i = 1; i < cc50.size(); ++i) {
    ULong64_t delta = ((cc50[i] - cc50[i - 1]) & constants::kROLLOVER50) * 20u;
    cc50[i - 1] = delta;
  }
  cc50.pop_back();
  ROOT::RDF::RNode output = input.Define(
      fName, [cc50](ULong64_t entry) -> ULong64_t { return entry == 0 ? 0 : cc50.at(entry - 1); }, {"rdfentry_"});
  return output;
}

REGISTER_STEP("delta_t", DeltaTStep);
} // namespace SnopAnalysis
