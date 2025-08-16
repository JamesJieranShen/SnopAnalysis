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
}

void
SnapshotWriter::Write(ROOT::RDF::RNode df) {
  TFile* file = TFile::Open(fFileName.c_str(), "RECREATE");
  if (!file || file->IsZombie()) {
    throw std::runtime_error("Failed to open output file: " + fFileName);
  }
  Logger::Info(std::format("Creating snapshot in file: {}", fFileName));

  if (fColumnNames.empty())
    df.Snapshot(fTreeName, fFileName);
  else {
    Logger::Info("Writing only selected columns: " + std::accumulate(fColumnNames.begin(), fColumnNames.end(),
                                                                     std::string(""),
                                                                     [](const std::string& a, const std::string& b) {
                                                                       return a + (a.length() > 0 ? ", " : "") + b;
                                                                     }));
    df.Snapshot(fTreeName, fFileName, fColumnNames);
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
