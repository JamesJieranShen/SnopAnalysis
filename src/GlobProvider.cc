#include "GlobProvider.hh"

#include "Logger.hh"
#include "StepRegistry.hh"

#include <CLI/CLI.hpp>
#include <TChainProvider.hh>

namespace SnopAnalysis {
void
GlobProvider::Configure(const nlohmann::json& config) {
  InputProvider::Configure(config);
  std::string glob = config.value("pattern", "");
  std::string tree_name = config.value("tree_name", "output");
  CLI::App app{"glob provider parser"};
  app.allow_extras();
  app.add_option("-i,--input", glob, "glob pattern for input files");
  app.add_option("--tree", tree_name, "tree name");
  app.parse(GetContext()->fCmdLine);
  Logger::Info("Using glob pattern: {}", glob);
  Logger::Info("Using tree name: {}", tree_name);
  nlohmann::json cfg = {{"tree_name", tree_name}, {"files", {glob}}, {"sort", true}};
  fChain = TChainProvider::GetChain(cfg);
}

ROOT::RDataFrame
GlobProvider::Get() {
  ROOT::RDataFrame df(*fChain);
  return df;
}

REGISTER_INPUT_PROVIDER("glob", GlobProvider);
} // namespace SnopAnalysis
