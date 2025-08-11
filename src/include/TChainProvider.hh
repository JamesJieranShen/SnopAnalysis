#pragma once

#include "InputProvider.hh"

#include <TChain.h>
#include <nlohmann/json.hpp>

namespace SnopAnalysis {
class TChainProvider : public InputProvider {
public:
  void Configure(const nlohmann::json& config) override;
  ROOT::RDataFrame Get() override;

private:
  std::unique_ptr<TChain> fChain;
  std::unique_ptr<TChain> GetChain(const nlohmann::json& cfg, bool run_sort);
};

} // namespace SnopAnalysis
