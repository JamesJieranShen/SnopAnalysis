#include "ResetStep.hh"

#include "StepRegistry.hh"

namespace SnopAnalysis {

void
ResetStep::Configure(const nlohmann::json& config) {
  Step::Configure(config);
  fPreserveMT = true;
  if (config.contains("multithreading")) {
    fPreserveMT = false;
    fMTOn = config["multithreading"];
  }
}

ROOT::RDF::RNode
ResetStep::DoExecute([[maybe_unused]] ROOT::RDF::RNode input) {
  if (!fPreserveMT) {
    if (fMTOn) {
      if (fContext->mt_threads > 1) {
        ROOT::EnableImplicitMT(fContext->mt_threads);
        Logger::Info(std::format("Multithreading enabled by Step {} with {} threads", fStepID, fContext->mt_threads));
      } else {
        Logger::Info(std::format("Multithreading requested by Step {} but multithreading has been disabled.", fStepID));
      }
    } else {
      ROOT::DisableImplicitMT();
      Logger::Info(std::format("Multithreading disabled by Step {}", fStepID));
    }
  }
  return ROOT::RDataFrame(0);
}

REGISTER_STEP("reset", ResetStep);
} // namespace SnopAnalysis
