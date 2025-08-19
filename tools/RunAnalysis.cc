#include "Context.hh"
#include "Logger.hh"
#include "StepRegistry.hh"

#include <CLI/CLI.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace SnopAnalysis;

int
main(int argc, char** argv) {
  CLI::App parser{"Run Analyis"};
  std::string cfg_fname;
  parser.add_option("config_file", cfg_fname, "Path to the configuration file")->required()->check(CLI::ExistingFile);

  Logger::Verbosity verbosity = Logger::Verbosity::Info;
  parser.add_option("-v,--verbosity", verbosity, "Verbosity level")
      ->transform(CLI::CheckedTransformer(Logger::VerbosityMap, CLI::ignore_case))
      ->default_val(Logger::Verbosity::Info);
  bool mt = false;
  int threads = 0;
  parser.add_flag("--mt", mt, "Enable multithreading (via ROOT ImplicitMT)");
  parser.add_option("-j,--jobs,--threads", threads, "Number of threads to use")
      ->check(CLI::Range(1, 1024)); // NOTE: buy a new CPU immediately when this is updated.
  parser.allow_extras();

  CLI11_PARSE(parser, argc, argv);

  Logger::SetLevel(verbosity);

  std::chrono::time_point start = std::chrono::high_resolution_clock::now();

  if (!mt && threads > 1) {
    Logger::Warn("Number of threads specified but multithreading not enabled. Ignoring thread count.");
  }
  if (!mt) {
    Logger::Info("Running in single-threaded mode.");
    threads = 1;
  } else {
    if (!threads) {
      ROOT::EnableImplicitMT();
      threads = ROOT::GetThreadPoolSize();
      Logger::Info("ROOT ImplicitMT : ON (auto) -> {} threads", threads);
    } else {
      ROOT::EnableImplicitMT(threads);
      Logger::Info("ROOT ImplicitMT : ON -> {} threads", threads);
    }
  }

  // Read JSON configuration from file
  std::ifstream configFile(cfg_fname);
  if (!configFile.is_open()) {
    std::cerr << "Could not open config file: " << argv[1] << std::endl;
    return 1;
  }

  nlohmann::json config = nlohmann::json::parse(configFile,
                                                nullptr, // no parser callback
                                                true,    // allow exceptions
                                                true     // allow comments
  );
  std::shared_ptr<Context> theContext = std::make_shared<Context>(MakeContext(config, argc, argv));
  theContext->mt_threads = threads;

  StepRegistry& step_registry = StepRegistry::Instance();
  StepRegistry::Instance().SetContext(theContext);

  ROOT::RDF::RNode df = ROOT::RDataFrame(0);
  nlohmann::json step_configs = config.at("steps");
  Logger::Info("Found {} steps in configuration", step_configs.size());
  std::vector<std::unique_ptr<Step>> steps;
  for (const auto& sconf : step_configs) {
    steps.emplace_back(step_registry.Create(sconf));
  }
  for (auto& step : steps) {
    df = step->Execute(df);
  }
  for (auto& step : steps) {
    step->Report();
  }

  Logger::Info("Computation graph executed for {} times.", df.GetNRuns());
  std::chrono::time_point end = std::chrono::high_resolution_clock::now();
  Logger::Info("Analysis completed in {} seconds",
               std::chrono::duration_cast<std::chrono::seconds>(end - start).count());
  return 0;
}
