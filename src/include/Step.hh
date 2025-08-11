#pragma once

#include <Context.hh>
#include <Logger.hh>
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
  virtual void Configure(const nlohmann::json& config) { fComment = config.value("comment", ""); }
  ROOT::RDF::RNode operator()(ROOT::RDF::RNode input, bool eager = true) {
    if (eager) {
      Logger::Debug(
          std::format("Eagerly executing STEP {}: {} ({})", fStepID, demangle(typeid(*this).name()), fComment));
      ROOT::RDF::RNode output = DoExecute(input);
      Logger::Info(std::format(std::locale("en_US.UTF-8"), "Finished STEP {}: {} ({}). Dataframe now has {:L} entries",
                               fStepID, demangle(typeid(*this).name()), fComment, output.Count().GetValue()));
      return output;
    } else {
      return DoExecute(input);
    }
  }
  virtual ROOT::RDF::RNode DoExecute(ROOT::RDF::RNode input) = 0;

  void SetStepID(size_t stepID) { fStepID = stepID; }
  size_t GetStepID() const { return fStepID; }
  void SetContext(std::shared_ptr<const Context> ctx) { fContext = std::move(ctx); }
  std::shared_ptr<const Context> GetContext() const { return fContext; }
  void SetComment(const std::string& comment) { fComment = comment; }
  const std::string& GetComment() const { return fComment; }

protected:
  size_t fStepID = static_cast<size_t>(-1); // default invalid
  std::string fComment;
  std::shared_ptr<const Context> fContext;
};

} // namespace SnopAnalysis
