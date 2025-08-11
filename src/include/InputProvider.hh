#pragma once
#include <ROOT/RDataFrame.hxx>
#include <nlohmann/json.hpp>

namespace SnopAnalysis {
class InputProvider {
  public:
    virtual ~InputProvider() = default;
    virtual void Configure(const nlohmann::json &config) = 0;

    // the return copy here may seem bad but RDataFrame is designed to be cheap to copy
    virtual ROOT::RDataFrame Get() = 0;
};
} // namespace SnopAnalysis
