#pragma once

#include "InputProvider.hh"

#include <TChain.h>
#include <nlohmann/json.hpp>

namespace SnopAnalysis {
class TChainProvider : public InputProvider {
public:
  void Configure(const nlohmann::json& config) override;
  ROOT::RDataFrame Get() override;

  static std::unique_ptr<TChain> GetChain(const nlohmann::json& cfg);

protected:
  std::unique_ptr<TChain> fChain;
  std::vector<std::unique_ptr<TChain>> fFriends;
};

} // namespace SnopAnalysis
