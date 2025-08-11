#pragma once
#include "Context.hh"
#include "Logger.hh"
#include "Step.hh"

#include <functional>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>

namespace SnopAnalysis {
class StepRegistry {
public:
  using Creator = std::function<std::unique_ptr<Step>()>;
  static StepRegistry& Instance() {
    static StepRegistry instance;
    return instance;
  }
  void SetContext(std::shared_ptr<Context> ctx) { fContext = std::move(ctx); }
  void Register(const std::string& name, Creator creator) { fRegistry[name] = std::move(creator); }
  std::unique_ptr<Step> Create(const nlohmann::json& cfg) {
    std::string name = cfg.value("type", "unknown");
    auto it = fRegistry.find(name);
    if (it != fRegistry.end()) {
      std::unique_ptr<Step> step = it->second();
      step->SetStepID(fNextStepID++);
      step->SetContext(fContext);
      step->Configure(cfg);
      Logger::Info(std::format("Creating STEP {}: {} ({})", step->GetStepID(), name, step->GetComment()));
      Logger::Debug(std::format("Step configuration: {}", cfg.dump(2)));
      return step;
    }
    throw std::runtime_error("Step not found: " + name);
  }

private:
  std::map<std::string, Creator> fRegistry;
  std::shared_ptr<Context> fContext;
  size_t fNextStepID = 0;
};
} // namespace SnopAnalysis

#define REGISTER_STEP(TYPE_STRING, CLASSNAME)                                                                          \
  namespace {                                                                                                          \
  struct Register_##CLASSNAME {                                                                                        \
    Register_##CLASSNAME() {                                                                                           \
      StepRegistry::Instance().Register(TYPE_STRING, []() {                                                            \
        auto result = std::make_unique<CLASSNAME>();                                                                   \
        return result;                                                                                                 \
      });                                                                                                              \
    }                                                                                                                  \
  };                                                                                                                   \
  static Register_##CLASSNAME reg_instance_##CLASSNAME;                                                                \
  }
