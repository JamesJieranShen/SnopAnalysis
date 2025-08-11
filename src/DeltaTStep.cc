#include "DeltaTStep.hh"

#include "StepRegistry.hh"

namespace SnopAnalysis {

void
DeltaTStep::Configure(const nlohmann::json& config) {
  Step::Configure(config);
  fName = config.value("name", "delta_t");
  fSequentialOnly = true;
}

ROOT::RDF::RNode
DeltaTStep::DoExecute(ROOT::RDF::RNode input) {
  auto prev = std::make_shared<ULong64_t>(0);
  auto prevValid = std::make_shared<bool>(false);
  return input.Define(fName,
                      [prev, prevValid](const ULong64_t& clockCount50) -> ULong64_t {
                        if (!*prevValid) {
                          *prev = clockCount50;
                          *prevValid = true;
                          return 0ULL;
                        }
                        const ULong64_t delta = ((clockCount50 - *prev) & constants::kROLLOVER50) * 20u;
                        *prev = clockCount50;
                        return delta;
                      },
                      {"clockCount50"});
}

REGISTER_STEP("delta_t", DeltaTStep);
} // namespace SnopAnalysis
