#pragma once
#include <Rtypes.h>
#include <cstdint>

namespace SnopAnalysis {
namespace constants {
constexpr uint64_t kROLLOVER50 = 0x7FFFFFFFFFF;
} // namespace constants

inline ULong64_t
DeltaT_Clock50(ULong64_t ta, ULong64_t tb) {
  // Calculate the delta time in clock counts (50 MHz)
  return ((tb - ta) & constants::kROLLOVER50) * 20u;
}

} // namespace SnopAnalysis
