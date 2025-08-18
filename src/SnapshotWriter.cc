#include "SnapshotWriter.hh"

#include "Logger.hh"
#include "StepRegistry.hh"
#include "TObjString.h"

#include <locale>

namespace SnopAnalysis {
void
SnapshotWriter::Configure(const nlohmann::json& config) {
  OutputWriter::Configure(config);
  fTreeName = config.value("tree_name", "output");
  fFileName = config.value("file_name", "output.ntuple.root");
  fColumnNames = config.value("columns", std::vector<std::string>{});
  fOpts.fAutoFlush = config.value("autoflush", 0);
  fOpts.fLazy = config.value("lazy", true);
  fOpts.fCompressionLevel = config.value("compression_level", 1);
  fOpts.fMode = config.value("creation_mode", "RECREATE");
  fOpts.fOverwriteIfExists = config.value("overwrite_if_exists", false);
  fOpts.fSplitLevel = config.value("split_level", 99);
  std::string compressionAlgo = config.value("compression_algorithm", "ZLIB");
  if (compressionAlgo == "ZLIB")
    fOpts.fCompressionAlgorithm = ROOT::kZLIB;
  else if (compressionAlgo == "LZMA")
    fOpts.fCompressionAlgorithm = ROOT::kLZMA;
  else if (compressionAlgo == "LZ4")
    fOpts.fCompressionAlgorithm = ROOT::kLZ4;
  else if (compressionAlgo == "ZSTD")
    fOpts.fCompressionAlgorithm = ROOT::kZSTD;
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

  if (fColumnNames.empty())
    df.Snapshot(fTreeName, fFileName, "", fOpts);
  else {
    Logger::Info("Writing only selected columns: " + std::accumulate(fColumnNames.begin(), fColumnNames.end(),
                                                                     std::string(""),
                                                                     [](const std::string& a, const std::string& b) {
                                                                       return a + (a.length() > 0 ? ", " : "") + b;
                                                                     }));
    df.Snapshot(fTreeName, fFileName, fColumnNames, fOpts);
  }

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
