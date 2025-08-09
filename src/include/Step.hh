#pragma once

#include <ROOT/RDataFrame.hxx>
#include <nlohmann/json.hpp>

namespace SnopAnalysis {
class Step {
  public:
    virtual ~Step() = default;
    virtual void Configure(const nlohmann::json &config) = 0;
    virtual ROOT::RDF::RNode Execute(ROOT::RDF::RNode input) = 0;

};

} // namespace SnopAnalysis
