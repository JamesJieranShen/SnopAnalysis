#include "RunProvider.hh"

#include "Logger.hh"
#include "StepRegistry.hh"

#include <CLI/CLI.hpp>
#include <TChainElement.h>
#include <TChainProvider.hh>

namespace SnopAnalysis {
void
RunProvider::Configure(const nlohmann::json& config) {
  InputProvider::Configure(config);
  // format: Analysis15_bMR_r0000363272_s004_p008.ntuple.root
  std::string directory = config.value("directory", "");
  std::string module_name = config.value("module_name", "");
  std::string tree_name = config.value("tree_name", "output");
  int run_number = config.value("run_number", -9999);
  CLI::App app{"run provider parser"};
  app.allow_extras();
  if (run_number == -9999) {
    Logger::Info("No run number provided, using attempting to read command line.");
    app.add_option("-r,--run", run_number, "run number");
  }
  if (directory.empty()) {
    Logger::Info("No directory provided, using attempting to read command line.");
    app.add_option("-d,--directory", directory, "directory containing the ntuple files");
  }
  if (module_name.empty()) {
    Logger::Info("No module name provided, using attempting to read command line.");
    app.add_option("-m,--module", module_name, "module name");
  }
  app.parse(GetContext()->fCmdLine);
  Logger::Info("Using directory: {}", directory);
  Logger::Info("Using module name: {}", module_name);
  Logger::Info("Using run number: {}", run_number);
  nlohmann::json cfg = {{"tree_name", tree_name},
                        {"files", {std::format("{}/{}_r{:010d}_*.ntuple.root", directory, module_name, run_number)}},
                        {"sort", true}};
  fChain = TChainProvider::GetChain(cfg);
}

ROOT::RDataFrame
RunProvider::Get() {
  ROOT::RDataFrame df(*fChain);
  return df;
}

REGISTER_INPUT_PROVIDER("run", RunProvider);
} // namespace SnopAnalysis
