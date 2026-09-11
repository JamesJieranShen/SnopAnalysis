#include "DeltaTStep.hh"

#include "Logger.hh"
#include "StepRegistry.hh"
#include "util.hh"

#include <memory>
#include <vector>

namespace SnopAnalysis {

void
DeltaTStep::Configure(const nlohmann::json& config) {
  Step::Configure(config);
  fName = config.value("name", "delta_t");
  fSequentialOnly = true;
}

ROOT::RDF::RNode
DeltaTStep::DoExecute(ROOT::RDF::RNode input) {
  // delta_t only depends on the previous event, so the times are streamed rather than
  // materialised. The results are indexed by rdfentry_ directly, which stays correct if the
  // input is filtered: entries that never arrive keep their default and are never looked up.
  std::vector<ULong64_t> delta_ts;
  bool first = true;
  ULong64_t previous_time = 0;
  input.Foreach(
      [&](ULong64_t entry, ULong64_t current_time) {
        if (entry >= delta_ts.size()) delta_ts.resize(entry + 1, 0);
        delta_ts[entry] = first ? 0 : DeltaT_Clock50(previous_time, current_time);
        previous_time = current_time;
        first = false;
      },
      {"rdfentry_", "clockCount50"});

  return DefineColumn(
      input, fName,
      [deltas = std::make_shared<const std::vector<ULong64_t>>(std::move(delta_ts))](ULong64_t entry) {
        if (entry >= deltas->size()) {
          Logger::Warn("DeltaTStep: Entry {} not found in rdfentry_", entry);
          return ULong64_t(0);
        }
        return (*deltas)[entry];
      },
      {"rdfentry_"}, fRedefine);
}

REGISTER_STEP("delta_t", DeltaTStep);
} // namespace SnopAnalysis
