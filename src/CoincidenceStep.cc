#include "CoincidenceStep.hh"

namespace SnopAnalysis {
void
CoincidenceStep::Configure(const nlohmann::json& config) {
  fWindow = config["window"];
  fPromptTagName = config["prompt_tag_name"];
  fDelayedTagName = config["delayed_tag_name"];
  fPromptExpr = config["prompt_expr"];
  fDelayedExpr = config["delayed_expr"];
  fTimeColumn = config["time_column"];
  if (fPromptTagName == fDelayedTagName) {
    throw std::runtime_error("Prompt and delayed tag names must be different.");
  }
}

ROOT::RDF::RNode
CoincidenceStep::Execute(ROOT::RDF::RNode input) {
  ROOT::RDF::RNode result = input.Redefine(fTimeColumn, "static_cast<double>(" + fTimeColumn + ")");
  std::vector<double> promptTimes = result.Filter(fPromptExpr).Take<double>(fTimeColumn).GetValue();
  std::sort(promptTimes.begin(), promptTimes.end());
  result = result.Define(fPromptTagName, fPromptExpr);
  auto delayedLambda = [pts = std::make_shared<std::vector<double>>(std::move(promptTimes)), lookAhead = (fWindow < 0),
                        window = std::abs(fWindow)](double tt, bool isDelayCandidate) -> bool {
    if (!isDelayCandidate) return false;
    const std::vector<double>& pTimes = *pts;
    if (pTimes.empty()) return false;
    if (!lookAhead) { // follower comes after the prompt event.
      auto it = std::upper_bound(pTimes.begin(), pTimes.end(), tt);
      if (it == pTimes.begin()) return false;
      double thePromptTime = *std::prev(it);
      return (tt - thePromptTime <= window);
    } else { // the follower is actually before the "prompt" event.
      auto it = std::lower_bound(pTimes.begin(), pTimes.end(), tt);
      if (it == pTimes.end()) return false;
      double thePromptTime = *it;
      return (thePromptTime - tt <= window);
    }
  };
  result = result.Define(fDelayedTagName, fDelayedExpr);
  result = result.Redefine(fDelayedTagName, delayedLambda, {fTimeColumn, fDelayedTagName});
  return result;
}
} // namespace SnopAnalysis
