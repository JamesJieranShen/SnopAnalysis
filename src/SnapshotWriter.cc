#include "SnapshotWriter.hh"

#include "Logger.hh"
#include "StepRegistry.hh"
#include "TObjString.h"

#include <CLI/CLI.hpp>
#include <locale>

namespace SnopAnalysis {
void
SnapshotWriter::Configure(const nlohmann::json& config) {
  OutputWriter::Configure(config);
  fTreeName = config.value("tree_name", "output");
  fFileName = config.value("file_name", "");
  if (fFileName == "") {
    Logger::Info("No file name provided, using attempting to read command line.");
    CLI::App app{"run_number_parser"};
    app.add_option("-o,--output", fFileName, "output filename");
    app.allow_extras();
    app.parse(GetContext()->fCmdLine);
    Logger::Info("Using output file name: {}", fFileName);
  }
  fColumnNames = config.value("columns", std::vector<std::string>{});
  fPruneColumnNames = config.value("prune_columns", std::vector<std::string>{});
  if (fColumnNames.size() > 0 && fPruneColumnNames.size() > 0) {
    Logger::Warn("Both columns and prune_columns are set. The final list of columns will be fColumnNames - "
                 "fPruneColumnNames. Make sure this is what you want!");
  }
  fOpts.fAutoFlush = config.value("autoflush", 0);
  fOpts.fLazy = config.value("lazy", false);
  fOpts.fCompressionLevel = config.value("compression_level", 1);
  fOpts.fMode = config.value("creation_mode", "RECREATE");
  fOpts.fOverwriteIfExists = config.value("overwrite_if_exists", false);
  fOpts.fSplitLevel = config.value("split_level", 99);
  std::string compressionAlgo = config.value("compression_algorithm", "ZLIB");
  if (compressionAlgo == "ZLIB")
    fOpts.fCompressionAlgorithm = ROOT::RCompressionSetting::EAlgorithm::kZLIB;
  else if (compressionAlgo == "LZMA")
    fOpts.fCompressionAlgorithm = ROOT::RCompressionSetting::EAlgorithm::kLZMA;
  else if (compressionAlgo == "LZ4")
    fOpts.fCompressionAlgorithm = ROOT::RCompressionSetting::EAlgorithm::kLZ4;
  else if (compressionAlgo == "ZSTD")
    fOpts.fCompressionAlgorithm = ROOT::RCompressionSetting::EAlgorithm::kZSTD;
  else
    throw std::runtime_error("Unknown compression algorithm: " + compressionAlgo);
}

void
SnapshotWriter::Write(ROOT::RDF::RNode df) {
  TFile* file = TFile::Open(fFileName.c_str(), "RECREATE");
  if (!file || file->IsZombie()) {
    throw std::runtime_error("Failed to open output file: " + fFileName);
  }
  Logger::Info("Creating snapshot in file: {}", fFileName);
  std::vector<std::string> columnsToWrite = fColumnNames.empty() ? df.GetColumnNames() : fColumnNames;
  {
    std::vector<std::string> tempColumns;
    for (std::string& col : columnsToWrite) {
      if (std::find(fPruneColumnNames.begin(), fPruneColumnNames.end(), col) == fPruneColumnNames.end()) {
        tempColumns.push_back(col);
      } else {
        Logger::Debug("Pruning column: " + col);
      }
    }
    std::swap(columnsToWrite, tempColumns);
  }

  Logger::Info(
      "Writing only selected columns: " +
      std::accumulate(columnsToWrite.begin(), columnsToWrite.end(), std::string(""),
                      [](const std::string& a, const std::string& b) { return a + (a.length() > 0 ? ", " : "") + b; }));
  fResult = df.Snapshot(fTreeName, fFileName, columnsToWrite, fOpts);

  // Close the file
  file->Close();
  delete file;
}

void
SnapshotWriter::WriteContext() const {
  if (!fContext) {
    Logger::Warn("No context available to write");
  }
  nlohmann::json ctxJson = *fContext;
  ctxJson["step_id"] = GetStepID();
  TFile* file = TFile::Open(fFileName.c_str(), "UPDATE");
  if (!file || file->IsZombie()) {
    throw std::runtime_error("Failed to open output file for context writing: " + fFileName);
  }
  TObjString ctxStr(ctxJson.dump().c_str());
  ctxStr.Write("context", TObject::kOverwrite);
  file->Close();
  delete file;
}
REGISTER_STEP("snapshot", SnapshotWriter)

} // namespace SnopAnalysis
