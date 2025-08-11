#pragma once
#include "Step.hh"

namespace SnopAnalysis {
class AliasStep : public Step {
public:
  void Configure(const nlohmann::json& config) override;
  ROOT::RDF::RNode DoExecute(ROOT::RDF::RNode input) override;

private:
  std::string fName;  // name of the new colum
  std::string fAlias; // alias to give
};

} // namespace SnopAnalysis
