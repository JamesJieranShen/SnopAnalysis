#include <CLI/CLI.hpp>
#include <TFile.h>
#include <TTree.h>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

void
CheckRootFiles(const std::string& dirpath) {
  std::vector<std::string> bad_files;
  for (const auto& entry : fs::directory_iterator(dirpath)) {
    if (!entry.is_regular_file()) continue;

    const auto& path = entry.path();
    if (path.extension() != ".root") continue;

    // Try opening the file
    TFile file(path.string().c_str(), "READ");
    if (file.TestBit(TFile::kRecovered)) {
      std::cerr << "[BAD] File was recovered, rejecting\n";
      bad_files.push_back(path.string());
      continue;
    }

    if (file.IsZombie()) {
      std::cerr << "[BAD] Cannot open: " << path << "\n";
      bad_files.push_back(path.string());
      continue;
    }

    // Try getting the tree named "output"
    TTree* tree = nullptr;
    file.GetObject("output", tree);
    if (!tree) {
      std::cerr << "[BAD] No tree named 'output' in: " << path << "\n";
      bad_files.push_back(path.string());
      continue;
    }

    std::cout << "[OK] " << path << " has tree 'output' with " << tree->GetEntries() << " entries\n";
  }
  std::cout << "====================" << std::endl;
  std::cout << "all bad files: " << std::endl;
  for (std::string& path : bad_files) {
    std::cout << path << std::endl;
  }
}

int
main(int argc, char** argv) {
  CLI::App parser{"Check ROOT files and see if they are corrupted."};
  std::string dirpath;
  parser.add_option("directory", dirpath, "Path to the directory containing ROOT files")
      ->required()
      ->check(CLI::ExistingDirectory);
  CLI11_PARSE(parser, argc, argv);
  CheckRootFiles(dirpath);
}
