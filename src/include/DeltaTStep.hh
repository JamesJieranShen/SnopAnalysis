#pragma once
#include "Step.hh"

namespace SnopAnalysis {
class DeltaTStep : public Step {
public:
  void Configure(const nlohmann::json& config) override;
  ROOT::RDF::RNode DoExecute(ROOT::RDF::RNode input) override;

private:
  std::string fName; // name of the new colum
};

} // namespace SnopAnalysis
