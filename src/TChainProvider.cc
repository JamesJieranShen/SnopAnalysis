#include "TChainProvider.hh"

#include "Logger.hh"
#include "StepRegistry.hh"

#include <TChainElement.h>
#include <locale>

namespace SnopAnalysis {
void
TChainProvider::Configure(const nlohmann::json& config) {
  InputProvider::Configure(config);
  const nlohmann::json& parts = config["parts"];
  for (auto& ff : parts) {
    if (!fChain)
      fChain = GetChain(ff);
    else {
      fFriends.emplace_back(GetChain(ff));
    }
  }
  for (const std::unique_ptr<TChain>& ff : fFriends) {
    if (!fChain->AddFriend(ff.get())) {
      Logger::Die("Failed to add friend chain: {}", ff->GetName());
    }
  }
}

std::unique_ptr<TChain>
TChainProvider::GetChain(const nlohmann::json& cfg) {
  std::string tree_name = cfg.value("tree_name", "output");
  std::vector<std::string> filePatterns = cfg["files"].get<std::vector<std::string>>();
  bool run_sort = cfg.value("sort", false);
  std::unique_ptr<TChain> result = std::make_unique<TChain>(tree_name.c_str());
  for (const std::string& pattern : filePatterns) {
    // create a temporary TChain to get the list of files matching the pattern
    TChain tempChain(tree_name.c_str());
    tempChain.Add(pattern.c_str());

    // pull filenames out of the temp chain, sort them, and add them to the main chain.
    std::vector<std::string> filelist;
    TObjArray* lst = tempChain.GetListOfFiles();
    if (!lst) {
      Logger::Die("List of files from patten {} is somehow null. Something is very wrong.", pattern);
    }
    filelist.reserve(lst->GetEntries());
    for (int i = 0; i < lst->GetEntries(); ++i) {
      if (auto* el = dynamic_cast<TChainElement*>(lst->At(i))) {
        filelist.emplace_back(el->GetTitle());
      }
    }
    if (run_sort) std::sort(filelist.begin(), filelist.end());
    for (const std::string& file : filelist) {
      Logger::Trace("Adding file: {}", file);
      result->Add(file.c_str());
    }
    if (filelist.empty()) {
      Logger::Warn("No files matched the pattern: {}", pattern);
    } else {
      if (run_sort) {
        Logger::Info("Added {} files from pattern after lexicographical sorting: {}", filelist.size(), pattern);
      } else {
        Logger::Info("Added {} files from pattern: {}", filelist.size(), pattern);
      }
    }
  }
  return result;
}

ROOT::RDataFrame
TChainProvider::Get() {
  ROOT::RDataFrame df(*fChain);
  return df;
}

REGISTER_INPUT_PROVIDER("tchain", TChainProvider);
} // namespace SnopAnalysis
