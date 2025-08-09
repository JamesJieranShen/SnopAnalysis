#include "Context.hh"
#include "InputProviderRegistry.hh"
#include "Logger.hh"
#include "StepRegistry.hh"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace SnopAnalysis;

int
main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <json_config_file>" << std::endl;
    return 1;
  }
  Logger::SetLevel(Verbosity::Debug);

  // Read JSON configuration from file
  std::ifstream configFile(argv[1]);
  if (!configFile.is_open()) {
    std::cerr << "Could not open config file: " << argv[1] << std::endl;
    return 1;
  }

  nlohmann::json config;
  configFile >> config;
  auto theContext = std::make_shared<Context>(MakeContext(config, argc, argv));
  std::unique_ptr<InputProvider> provider = InputProviderRegistry::Instance().Create(config.at("input"));
  ROOT::RDF::RNode df = provider->Get();

  StepRegistry& step_registry = StepRegistry::Instance();
  StepRegistry::Instance().SetContext(theContext);
  nlohmann::json step_configs = config.at("steps");
  Logger::Info(std::format("Found {} steps in configuration", step_configs.size()));
  std::vector<std::unique_ptr<Step>> steps;
  for (const auto& sconf : step_configs) {
    steps.emplace_back(step_registry.Create(sconf));
  }
  for (auto &step: steps) {
    df = (*step)(df);
  }
  return 0;
}
