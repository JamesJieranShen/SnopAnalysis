#include "CoincidenceStep.hh"

#include <TVector3.h>

namespace SnopAnalysis {
void
CoincidenceStep::Configure(const nlohmann::json& config) {
  Step::Configure(config);
  fTimeWindow = config["time_window"];
  fDeltaRWindow = config.value("delta_r_limit", std::numeric_limits<double>::max());
  fPromptTagName = config["prompt_tag_name"];
  fDelayedTagName = config["delayed_tag_name"];
  fPromptExpr = config["prompt_expr"];
  fDelayedExpr = config["delayed_expr"];
  fTimeColumn = config["time_column"];
  fPosColumns = config.value("pos_columns", std::array<std::string, 3>{"posx", "posy", "posz"});
  if (fPromptTagName == fDelayedTagName) {
    throw std::runtime_error("Prompt and delayed tag names must be different.");
  }
}

ROOT::RDF::RNode
CoincidenceStep::DoExecute(ROOT::RDF::RNode input) {
  ROOT::RDF::RNode result = input.Redefine(fTimeColumn, "static_cast<double>(" + fTimeColumn + ")");
  ROOT::RDF::RNode prompts = result.Filter(fPromptExpr);
  std::vector<double> promptTimes = prompts.Take<double>(fTimeColumn).GetValue();
  std::vector<double> promptX = prompts.Take<double>(fPosColumns[0]).GetValue();
  std::vector<double> promptY = prompts.Take<double>(fPosColumns[1]).GetValue();
  std::vector<double> promptZ = prompts.Take<double>(fPosColumns[2]).GetValue();

  // sort xyzt parallel arrays by time
  std::vector<std::size_t> promptIdx(promptTimes.size());
  std::iota(promptIdx.begin(), promptIdx.end(), 0);
  std::sort(promptIdx.begin(), promptIdx.end(),
            [&promptTimes](std::size_t a, std::size_t b) { return promptTimes[a] < promptTimes[b]; });

  std::sort(promptTimes.begin(), promptTimes.end());
  auto apply_permutation = [&](std::vector<double>& vec) {
    std::vector<double> tmp;
    tmp.reserve(vec.size());
    for (size_t idx : promptIdx)
      tmp.push_back(vec[idx]);
    vec.swap(tmp);
  };
  apply_permutation(promptTimes);
  apply_permutation(promptX);
  apply_permutation(promptY);
  apply_permutation(promptZ);

  result = result.Define(fPromptTagName, fPromptExpr);
  auto delayedLambda = [
                           // make these shared pointers to avoid copying large vectors in each thread.
                           pts = std::make_shared<const std::vector<double>>(std::move(promptTimes)),
                           pxs = std::make_shared<const std::vector<double>>(std::move(promptX)),
                           pys = std::make_shared<const std::vector<double>>(std::move(promptY)),
                           pzs = std::make_shared<const std::vector<double>>(std::move(promptZ)),
                           lookAhead = (fTimeWindow < 0),
                           // NOTE: do we need to have minimum dt and dr?
                           max_dt = std::abs(fTimeWindow),
                           max_dr2 = fDeltaRWindow * fDeltaRWindow // compare squared value since sqrt is slow
  ](double tt, double xx, double yy, double zz, bool isDelayCandidate) -> bool {
    if (!isDelayCandidate) return false; // current event is not a delayed candidate
    const std::vector<double>& pTimes = *pts;
    const std::vector<double>& pPosXs = *pxs;
    const std::vector<double>& pPosYs = *pys;
    const std::vector<double>& pPosZs = *pzs;
    if (pTimes.empty()) return false; // no prompt candidates available

    // binary search for lower and upper bounds in time.
    auto lo_it = pTimes.begin();
    auto hi_it = pTimes.end();
    if (lookAhead) { // search in bound [tt, tt + max_dt]
      lo_it = std::lower_bound(pTimes.begin(), pTimes.end(), tt);
      hi_it = std::upper_bound(pTimes.begin(), pTimes.end(), tt + max_dt);
    } else { // search in bound [tt - max_dt, tt]
      lo_it = std::lower_bound(pTimes.begin(), pTimes.end(), tt - max_dt);
      hi_it = std::upper_bound(pTimes.begin(), pTimes.end(), tt);
    }

    if (lo_it >= hi_it) return false; // no candidates in time window
    const size_t lo = std::distance(pTimes.begin(), lo_it);
    const size_t hi = std::distance(pTimes.begin(), hi_it);
    for (size_t idx = lo; idx < hi; ++idx) {
      const double dx = xx - pPosXs[idx];
      const double dy = yy - pPosYs[idx];
      const double dz = zz - pPosZs[idx];
      const double dr2 = dx * dx + dy * dy + dz * dz;
      if (dr2 <= max_dr2) return true;
    }
    return false;
  };
  result = result.Define(fDelayedTagName, fDelayedExpr);
  result = result.Redefine(fDelayedTagName, delayedLambda,
                           {fTimeColumn, fPosColumns[0], fPosColumns[1], fPosColumns[2], fDelayedTagName});
  return result;
}
} // namespace SnopAnalysis
