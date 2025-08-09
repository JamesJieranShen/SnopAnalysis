#pragma once
#include "InputProvider.hh"
#include <functional>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>

namespace SnopAnalysis {
class InputProviderRegistry {
public:
  using Creator =
      std::function<std::unique_ptr<InputProvider>(const nlohmann::json &)>;
  static InputProviderRegistry &Instance() {
    static InputProviderRegistry instance;
    return instance;
  }
  void Register(const std::string &name, Creator creator) {
    fRegistry[name] = std::move(creator);
  }
  std::unique_ptr<InputProvider> Create(const nlohmann::json &cfg) {
    std::string name = cfg.value("type", "unknown");
    auto it = fRegistry.find(name);
    if (it != fRegistry.end()) {
      return it->second(cfg);
    }
    throw std::runtime_error("InputProvider not found: " + name);
  }

private:
  std::map<std::string, Creator> fRegistry;
};
} // namespace SnopAnalysis

#define REGISTER_INPUT_PROVIDER(TYPE_STRING, CLASSNAME)                        \
  namespace {                                                                  \
  struct Register_##CLASSNAME {                                                \
    Register_##CLASSNAME() {                                                   \
      InputProviderRegistry::Instance().Register(                              \
          TYPE_STRING, [](const nlohmann::json &cfg) {                         \
            auto result = std::make_unique<CLASSNAME>();                       \
            result->Configure(cfg);                                            \
            return result;                                                     \
          });                                                                  \
    }                                                                          \
  };                                                                           \
  static Register_##CLASSNAME reg_instance_##CLASSNAME;                        \
  }
