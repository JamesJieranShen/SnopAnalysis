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
}

ROOT::RDataFrame
FileListProvider::Get() {
  Logger::Debug(std::format("FileInputProvider created dataframe with {:L} entries.", fDataFrame->Count().GetValue()));
  return *fDataFrame;
}

REGISTER_INPUT_PROVIDER("file_list", FileListProvider);
} // namespace SnopAnalysis
