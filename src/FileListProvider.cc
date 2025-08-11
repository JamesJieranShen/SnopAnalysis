#include "FileListProvider.hh"

#include "InputProviderRegistry.hh"
#include "Logger.hh"

namespace SnopAnalysis {
void
FileListProvider::Configure(const nlohmann::json& config) {
  fTreeName = config.value("tree_name", "output");
  fFileList = config.value("files", std::vector<std::string>{});
  if (fFileList.empty()) {
    Logger::Warn("FileListInputProvider: No files provided in the configuration.");
    throw std::runtime_error("FileListInputProvider: No files provided in the configuration.");
  }

  fDataFrame = std::make_unique<ROOT::RDataFrame>(fTreeName, fFileList);
  Logger::Info(std::format("FileListInputProvider initialized with {} entries.", fDataFrame->Count().GetValue()));
}

ROOT::RDataFrame&
FileListProvider::Get() {
  return *fDataFrame;
}

REGISTER_INPUT_PROVIDER("file_list", FileListProvider);
} // namespace SnopAnalysis
