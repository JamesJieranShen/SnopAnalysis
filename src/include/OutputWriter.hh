#pragma once
#include "Step.hh"

#include <Logger.hh>
#include <ROOT/RDataFrame.hxx>
#include <nlohmann/json.hpp>

namespace SnopAnalysis {
class OutputWriter : public Step {
public:
  virtual ROOT::RDF::RNode DoExecute(ROOT::RDF::RNode input) override {
    // Straight passthrough, but we call Write on the input
    Write(input);
    if (fWriteContext) WriteContext();
    return input;
  }
  virtual void Configure(const nlohmann::json& config) override {
    fWriteContext = config.value("write_context", true);
  }
  virtual void Write(ROOT::RDF::RNode df) = 0;
  virtual void WriteContext() const = 0;

private:
  bool fWriteContext;
};
} // namespace SnopAnalysis
