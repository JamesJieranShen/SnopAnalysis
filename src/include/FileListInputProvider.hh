#pragma once

#include "InputProvider.hh"
#include <nlohmann/json.hpp>

namespace SnopAnalysis {
class FileListInputProvider : public InputProvider {
public:
  void Configure(const nlohmann::json &config) override;
  ROOT::RDataFrame &Get() override;

private:
  std::string fTreeName;
  std::vector<std::string> fFileList;
  std::unique_ptr<ROOT::RDataFrame> fDataFrame;
};

} // namespace SnopAnalysis
