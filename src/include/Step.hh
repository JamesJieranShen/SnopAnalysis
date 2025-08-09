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
  virtual void Configure(const nlohmann::json& config) = 0;
  ROOT::RDF::RNode operator()(ROOT::RDF::RNode input) {
    Logger::Info(std::format("Executing STEP {}: {}", fStepID, demangle(typeid(*this).name())));
    return DoExecute(input);
  }
  virtual ROOT::RDF::RNode DoExecute(ROOT::RDF::RNode input) = 0;
  void SetStepID(size_t stepID) { fStepID = stepID; }
  size_t GetStepID() const { return fStepID; }
  void SetContext(std::shared_ptr<const Context> ctx) { fContext = std::move(ctx); }

protected:
  size_t fStepID = static_cast<size_t>(-1); // default invalid
  std::shared_ptr<const Context> fContext;
};

} // namespace SnopAnalysis
