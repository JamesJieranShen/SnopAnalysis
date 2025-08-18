#pragma once

#include "TChainProvider.hh"

#include <TChain.h>
#include <nlohmann/json.hpp>

namespace SnopAnalysis {
class RunProvider : public TChainProvider {
public:
  void Configure(const nlohmann::json& config) override;
  ROOT::RDataFrame Get() override;

private:
  std::unique_ptr<TChain> fChain;
};

} // namespace SnopAnalysis
