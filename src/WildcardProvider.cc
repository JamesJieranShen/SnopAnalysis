#include "WildcardProvider.hh"

#include "InputProviderRegistry.hh"
#include "Logger.hh"

#include <TChainElement.h>
#include <locale>

namespace SnopAnalysis {
void
WildcardProvider::Configure(const nlohmann::json& config) {
  fTreeName = config.value("tree_name", "output");
  std::vector<std::string> filePatterns = config["patterns"].get<std::vector<std::string>>();
  fChain = std::make_unique<TChain>(fTreeName.c_str());

  bool run_sort = config.value("sort", false);
  for (const std::string& pattern : filePatterns) {
    // create a temporary TChain to get the list of files matching the pattern
    TChain tempChain(fTreeName.c_str());
    tempChain.Add(pattern.c_str());

    // pull filenames out of the temp chain, sort them, and add them to the main chain.
    std::vector<std::string> filelist;
    TObjArray* lst = tempChain.GetListOfFiles();
    if (!lst) {
      Logger::Die(std::format("List of files from patten {} is somehow null. Something is very wrong.", pattern));
    }
    filelist.reserve(lst->GetEntries());
    for (int i = 0; i < lst->GetEntries(); ++i) {
      if (auto* el = dynamic_cast<TChainElement*>(lst->At(i))) {
        filelist.emplace_back(el->GetTitle());
      }
    }
    if (run_sort) std::sort(filelist.begin(), filelist.end());
    for (const std::string& file : filelist) {
      Logger::Trace(std::format("Adding file: {}", file));
      fChain->Add(file.c_str());
    }
    if (filelist.empty()) {
      Logger::Warn(std::format("No files matched the pattern: {}", pattern));
    } else {
      if (run_sort) {
        Logger::Info(
            std::format("Added {} files from pattern after lexicographical sorting: {}", filelist.size(), pattern));
      } else {
        Logger::Info(std::format("Added {} files from pattern: {}", filelist.size(), pattern));
      }
    }
  }
}

ROOT::RDataFrame
WildcardProvider::Get() {
  ROOT::RDataFrame df(*fChain);
  Logger::Info(std::format(std::locale("en_US.UTF-8"), "Created RDataFrame with {:L} entries.", df.Count().GetValue()));
  return df;
}

REGISTER_INPUT_PROVIDER("wildcard", WildcardProvider);
} // namespace SnopAnalysis
