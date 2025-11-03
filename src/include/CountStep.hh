#pragma once
#include "Step.hh"

namespace SnopAnalysis {
class CountStep : public Step {
public:
  void Configure(const nlohmann::json& config) override;
  ROOT::RDF::RNode DoExecute(ROOT::RDF::RNode input) override;
  void DoReport() override;

private:
  std::string fName;
  std::string fTotalCountName;
  std::string fMCCountName;
  bool fCountTotal, fCountMC;
  ROOT::RDF::RResultPtr<ULong64_t> fTotalCountResult;
};

} // namespace SnopAnalysis
