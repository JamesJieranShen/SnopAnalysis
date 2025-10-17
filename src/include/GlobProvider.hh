#include "InputProvider.hh"
namespace SnopAnalysis {
class GlobProvider : public InputProvider {
public:
  void Configure(const nlohmann::json& config) override;
  ROOT::RDataFrame Get() override;

private:
  std::unique_ptr<TChain> fChain;
};
} // namespace SnopAnalysis
