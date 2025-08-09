#include "SnapshotWriter.hh"

#include "Logger.hh"
#include "OutputWriterRegistry.hh"
#include "TObjString.h"

namespace SnopAnalysis {
void
SnapshotWriter::Configure(const nlohmann::json& config) {
  fTreeName = config.value("tree_name", "output");
  fFileName = config.value("file_name", "output.ntuple.root");
}

void
SnapshotWriter::Write(ROOT::RDF::RNode df) {
  TFile* file = TFile::Open(fFileName.c_str(), "RECREATE");
  if (!file || file->IsZombie()) {
    throw std::runtime_error("Failed to open output file: " + fFileName);
  }
  Logger::Info(std::format("Creating snapshot in file: {}", fFileName));

  // Add branches to the tree based on the dataframe schema
  df.Snapshot(fTreeName, fFileName);

  // Close the file
  file->Close();
  delete file;
}

void
SnapshotWriter::WriteConfig(nlohmann::json& config) {
  TFile* file = TFile::Open(fFileName.c_str(), "UPDATE");
  TObjString cfg_string(config.dump().c_str());
  cfg_string.Write("config", TObject::kOverwrite);
  file->Close();
  delete file;
}
REGISTER_OUTPUT_WRITER("snapshot", SnapshotWriter)

} // namespace SnopAnalysis
