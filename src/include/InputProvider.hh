#pragma once
#include "Step.hh"

#include <ROOT/RDataFrame.hxx>
#include <nlohmann/json.hpp>

namespace SnopAnalysis {
class InputProvider : public Step {
public:
  virtual void Configure(const nlohmann::json& config) override { Step::Configure(config); }
  virtual ROOT::RDF::RNode DoExecute(ROOT::RDF::RNode input) override {
    if (input.Count().GetValue() > 0) {
      Logger::Info("InputProvider called on non-empty dataframe. Previous data is now discarded.");
    }
    return Get();
  }

protected:
  // the return copy here may seem bad but RDataFrame is designed to be cheap to copy
  virtual ROOT::RDataFrame Get() = 0;
};
} // namespace SnopAnalysis
