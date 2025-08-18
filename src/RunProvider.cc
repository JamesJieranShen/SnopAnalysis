#include "RunProvider.hh"

#include "Logger.hh"
#include "StepRegistry.hh"

#include <CLI/CLI.hpp>
#include <TChainElement.h>
#include <locale>

namespace SnopAnalysis {
void
RunProvider::Configure(const nlohmann::json& config) {
  InputProvider::Configure(config);
  // format: Analysis15_bMR_r0000363272_s004_p008.ntuple.root
  std::string directory = config.value("directory", "");
  std::string module_name = config.value("module_name", "Analysis15_bMR");
  std::string tree_name = config.value("tree_name", "output");
  int run_number = config.value("run_number", -9999);
  if (run_number <= 0) {
    Logger::Info("No run number provided, using attempting to read command line.");
    CLI::App app{"run_number_parser"};
    app.add_option("-r,--run", run_number, "run number");
    app.allow_extras();
    app.parse(GetContext()->fCmdLine);
    Logger::Info("Using run number: {}", run_number);
  }
  nlohmann::json cfg = {{"tree_name", tree_name},
                        {"files", {std::format("{}/{}_r{:10d}_*.ntuple.root", directory, module_name, run_number)}},
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
