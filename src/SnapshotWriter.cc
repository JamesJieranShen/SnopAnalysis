#include "SnapshotWriter.hh"

#include "Logger.hh"
#include "StepRegistry.hh"
#include "TObjString.h"

namespace SnopAnalysis {
void
SnapshotWriter::Configure(const nlohmann::json& config) {
  OutputWriter::Configure(config);
  fTreeName = config.value("tree_name", "output");
  fFileName = config.value("file_name", "output.ntuple.root");
}

void
SnapshotWriter::Write(ROOT::RDF::RNode df) {
  TFile* file = TFile::Open(fFileName.c_str(), "RECREATE");
  if (!file || file->IsZombie()) {
    throw std::runtime_error("Failed to open output file: " + fFileName);
  }
  Logger::Info(std::format("Creating snapshot in file: {} -- {} entries.", fFileName, df.Count().GetValue()));

  // Add branches to the tree based on the dataframe schema
  df.Snapshot(fTreeName, fFileName);

  // Close the file
  file->Close();
  delete file;
}

void
SnapshotWriter::WriteContext() const {
  if (!fContext) {
    Logger::Warning("No context available to write");
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
