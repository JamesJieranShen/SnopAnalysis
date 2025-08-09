#include "InputProviderRegistry.hh"
#include "OutputWriterRegistry.hh"
#include "Logger.hh"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using namespace SnopAnalysis;

int main(int argc, char** argv) {
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

    nlohmann::json configJson;
    configFile >> configJson;

    // Create FileListInputProvider from JSON configuration
    std::unique_ptr<InputProvider> provider = InputProviderRegistry::Instance().Create(configJson.at("input"));
    ROOT::RDataFrame& df = provider->Get();
    std::unique_ptr<OutputWriter> writer = OutputWriterRegistry::Instance().Create(configJson.at("output"));
    writer->Write(df);
    writer->WriteConfig(configJson);
    return 0;
}
