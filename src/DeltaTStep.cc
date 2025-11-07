#include "DeltaTStep.hh"

#include "Logger.hh"
#include "StepRegistry.hh"
#include "util.hh"

namespace SnopAnalysis {

void
DeltaTStep::Configure(const nlohmann::json& config) {
  Step::Configure(config);
  fName = config.value("name", "delta_t");
  fSequentialOnly = true;
}

ROOT::RDF::RNode
DeltaTStep::DoExecute(ROOT::RDF::RNode input) {
  auto df_cached = input.Cache({"clockCount50"});
  std::vector<ULong64_t> rdfentry = input.Take<ULong64_t>("rdfentry_").GetValue();
  std::vector<ULong64_t> times = df_cached.Take<ULong64_t>("clockCount50").GetValue();
  std::vector<ULong64_t> delta_ts;
  delta_ts.reserve(times.size());
  ULong64_t previous_time = 99999;
  for (ULong64_t current_time : times) {
    if (previous_time == 99999) {
      delta_ts.push_back(0);
    } else {
      ULong64_t delta_t = DeltaT_Clock50(previous_time, current_time);
      delta_ts.push_back(delta_t);
    }
    previous_time = current_time;
  }
  auto result = input.Define(fName,
                             [rdfentry, delta_ts](ULong64_t entry) {
                               auto it = std::lower_bound(rdfentry.begin(), rdfentry.end(), entry);
                               if (it != rdfentry.end() && *it == entry) {
                                 size_t index = std::distance(rdfentry.begin(), it);
                                 return delta_ts[index];
                               } else {
                                 Logger::Warn("DeltaTStep: Entry {} not found in rdfentry_", entry);
                                 return ULong64_t(0);
                               }
                             },
                             {"rdfentry_"});
  return result;
}

REGISTER_STEP("delta_t", DeltaTStep);
} // namespace SnopAnalysis
