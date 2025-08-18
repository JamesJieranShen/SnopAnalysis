#include "CoincidenceStep.hh"

#include "StepRegistry.hh"
#include "util.hh"

#include <TVector3.h>
#include <chrono>

namespace SnopAnalysis {
void
CoincidenceStep::Configure(const nlohmann::json& config) {
  fSequentialOnly = true;
  Step::Configure(config);
  fTimeWindow = config["time_window"];
  fDeltaRWindow = config.value("delta_r_limit", std::numeric_limits<double>::max());
  fPromptTagName = config["label"].get<std::string>() + "_prompt";
  fDelayedTagName = config["label"].get<std::string>() + "_follower";
  fPromptExpr = config["prompt_expr"];
  fDelayedExpr = config["delayed_expr"];
  fPosColumns = config.value("pos_columns", std::array<std::string, 3>{"posx", "posy", "posz"});
  if (fPromptTagName == fDelayedTagName) {
    throw std::runtime_error("Prompt and delayed tag names must be different.");
  }
}

ROOT::RDF::RNode
CoincidenceStep::DoExecute(ROOT::RDF::RNode input) {
  auto start = std::chrono::high_resolution_clock::now();
  auto df_candidates =
      input.Define(fPromptTagName, fPromptExpr)
          .Define(fDelayedTagName, fDelayedExpr)
          .Cache({fPosColumns[0], fPosColumns[1], fPosColumns[2], "clockCount50", fPromptTagName, fDelayedTagName});
  std::vector<double> posX = df_candidates.Take<double>(fPosColumns[0]).GetValue();
  std::vector<double> posY = df_candidates.Take<double>(fPosColumns[1]).GetValue();
  std::vector<double> posZ = df_candidates.Take<double>(fPosColumns[2]).GetValue();
  std::vector<ULong64_t> time = df_candidates.Take<ULong64_t>("clockCount50").GetValue();
  std::vector<bool> prompt_candidate = df_candidates.Take<bool>(fPromptTagName).GetValue();
  std::vector<bool> delayed_candidate = df_candidates.Take<bool>(fDelayedTagName).GetValue();
  std::vector<size_t> prompt_indices;
  std::vector<size_t> delayed_indices;
  for (size_t iPrompt = 0; iPrompt < time.size(); ++iPrompt) {
    if (!prompt_candidate[iPrompt]) continue;
    double pX = posX[iPrompt];
    double pY = posY[iPrompt];
    double pZ = posZ[iPrompt];
    ULong64_t pTime = time[iPrompt];
    for (size_t iDelayed = iPrompt; iDelayed < time.size(); ++iDelayed) {
      if (!delayed_candidate[iDelayed]) continue;
      ULong64_t dTime = DeltaT_Clock50(pTime, time[iDelayed]);
      if (dTime > fTimeWindow) break; // No need to check further
      double dX = posX[iDelayed] - pX;
      double dY = posY[iDelayed] - pY;
      double dZ = posZ[iDelayed] - pZ;
      double dR = std::sqrt(dX * dX + dY * dY + dZ * dZ);
      if (dR < fDeltaRWindow) { // qualifying pair found
        prompt_indices.push_back(iPrompt);
        delayed_indices.push_back(iDelayed);
        break; // remove this line if search should continue for more delayed candidates
      }
    }
  }
  Logger::Debug("Found {} prompt candidates and {} delayed candidates", prompt_indices.size(), delayed_indices.size());
  Logger::Debug(
      "Coincidence step took {} ms",
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count());
  sort(prompt_indices.begin(), prompt_indices.end());
  sort(delayed_indices.begin(), delayed_indices.end());
  // NOTE: may be faster to use a unordered_set
  auto result = input.Define(fPromptTagName,
                             [pIdxs = std::make_shared<const std::vector<size_t>>(std::move(prompt_indices))](
                                 ULong64_t idx) { return (std::binary_search(pIdxs->begin(), pIdxs->end(), idx)); },
                             {"rdfentry_"});
  result = result.Define(fDelayedTagName,
                         [dIdxs = std::make_shared<const std::vector<size_t>>(std::move(delayed_indices))](
                             ULong64_t idx) { return (std::binary_search(dIdxs->begin(), dIdxs->end(), idx)); },
                         {"rdfentry_"});
  return result;
}
REGISTER_STEP("coincidence", CoincidenceStep);
} // namespace SnopAnalysis
