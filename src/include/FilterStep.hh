#pragma once
#include "Step.hh"

namespace SnopAnalysis {
class FilterStep : public Step {
public:
  void Configure(const nlohmann::json &config) override;
  ROOT::RDF::RNode Execute(ROOT::RDF::RNode input) override;

private:
  std::string fName;                 // name of the new colum
  std::string fExpression;           // expression for the new column
};

} // namespace SnopAnalysis
