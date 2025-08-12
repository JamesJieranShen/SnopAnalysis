#pragma once
#include "Step.hh"

namespace SnopAnalysis {
class CoincidenceStep : public Step {
public:
  void Configure(const nlohmann::json &config) override;
  ROOT::RDF::RNode DoExecute(ROOT::RDF::RNode input) override;

private:
  double fTimeWindow;
  double fDeltaRWindow;
  std::string fPromptTagName, fDelayedTagName;
  std::string fPromptExpr, fDelayedExpr;
  std::string fTimeColumn;
  std::array<std::string, 3> fPosColumns;
};

} // namespace SnopAnalysis
