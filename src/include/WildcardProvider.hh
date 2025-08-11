#pragma once

#include "InputProvider.hh"

#include <TChain.h>
#include <nlohmann/json.hpp>

namespace SnopAnalysis {
class WildcardProvider : public InputProvider {
public:
  void Configure(const nlohmann::json& config) override;
  ROOT::RDataFrame Get() override;

private:
  std::string fTreeName;
  std::unique_ptr<TChain> fChain;
};

} // namespace SnopAnalysis
