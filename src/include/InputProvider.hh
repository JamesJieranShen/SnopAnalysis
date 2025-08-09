#pragma once
#include <ROOT/RDataFrame.hxx>
#include <nlohmann/json.hpp>

namespace SnopAnalysis {
class InputProvider {
  public:
    virtual ~InputProvider() = default;
    virtual void Configure(const nlohmann::json &config) = 0;

    virtual ROOT::RDataFrame& Get() = 0;

};
} // namespace SnopAnalysis
