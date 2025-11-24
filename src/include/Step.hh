#pragma once

#include <Context.hh>
#include <Logger.hh>
#include <ROOT/RDFHelpers.hxx>
#include <ROOT/RDataFrame.hxx>
#include <cxxabi.h>
#include <nlohmann/json.hpp>

inline std::string
demangle(const char* mangled) {
  int status = 0;
  char* demangled = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);
  std::string result = (status == 0 && demangled) ? demangled : mangled;
  std::free(demangled);
  return result;
}

namespace SnopAnalysis {
class Step {
public:
  virtual ~Step() = default;
  virtual void Configure(const nlohmann::json& config) {
    fComment = config.value("comment", "");
    fSequentialOnly = config.value("sequential_only", false);
  }
  ROOT::RDF::RNode Execute(ROOT::RDF::RNode input) {
    if (fSequentialOnly && ROOT::IsImplicitMTEnabled()) {
      Logger::Die("This step is configured to run sequentially, but implicit multithreading is enabled. "
                  "Please disable implicit multithreading or set 'sequential_only' to false in the configuration.");
    }
    Logger::Info("Starting STEP {}: {} ({}).", fStepID, demangle(typeid(*this).name()), fComment);
    auto result = DoExecute(input);
    ROOT::RDF::Experimental::AddProgressBar(result);
    Logger::Info("Finished STEP {}: {} ({}).", fStepID, demangle(typeid(*this).name()), fComment);
    return result;
  }
  void Report() {
    Logger::Debug("Reporting step {}: {} ({}).", fStepID, demangle(typeid(*this).name()), fComment);
    DoReport();
  }

  void SetStepID(size_t stepID) { fStepID = stepID; }
  size_t GetStepID() const { return fStepID; }
  void SetContext(std::shared_ptr<const Context> ctx) { fContext = std::move(ctx); }
  std::shared_ptr<const Context> GetContext() const { return fContext; }
  void SetComment(const std::string& comment) { fComment = comment; }
  const std::string& GetComment() const { return fComment; }

protected:
  virtual ROOT::RDF::RNode DoExecute(ROOT::RDF::RNode input) = 0;
  virtual void DoReport(){};
  size_t fStepID = static_cast<size_t>(-1); // default invalid
  std::string fComment;
  bool fSequentialOnly = false;
  std::shared_ptr<const Context> fContext;
};

} // namespace SnopAnalysis
