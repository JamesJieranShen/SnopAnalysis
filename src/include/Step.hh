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

/// Define a column, overwriting an existing one of the same name when the step opts in.
/// Define throws if the column already exists and Redefine throws if it does not, so a step that
/// may run over a file already carrying its output has to pick based on what is actually present.
template <typename Expr>
inline ROOT::RDF::RNode
DefineColumn(ROOT::RDF::RNode node, const std::string& name, Expr&& expr, bool overwrite) {
  if (overwrite && node.HasColumn(name)) {
    Logger::Debug("Overwriting existing column {}", name);
    return node.Redefine(name, std::forward<Expr>(expr));
  }
  return node.Define(name, std::forward<Expr>(expr));
}

template <typename Expr>
inline ROOT::RDF::RNode
DefineColumn(ROOT::RDF::RNode node, const std::string& name, Expr&& expr, const ROOT::RDF::ColumnNames_t& columns,
             bool overwrite) {
  if (overwrite && node.HasColumn(name)) {
    Logger::Debug("Overwriting existing column {}", name);
    return node.Redefine(name, std::forward<Expr>(expr), columns);
  }
  return node.Define(name, std::forward<Expr>(expr), columns);
}

class Step {
public:
  virtual ~Step() = default;
  virtual void Configure(const nlohmann::json& config) {
    fComment = config.value("comment", "");
    fSequentialOnly = config.value("sequential_only", false);
    fRedefine = config.value("redefine", false);
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
  // Honoured only by steps that add columns; pass it to DefineColumn above.
  bool fRedefine = false;
  std::shared_ptr<const Context> fContext;
};

} // namespace SnopAnalysis
