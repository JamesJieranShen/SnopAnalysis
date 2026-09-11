#include "CoincidenceStep.hh"

#include "StepRegistry.hh"
#include "util.hh"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <deque>
#include <limits>
#include <memory>
#include <vector>

namespace SnopAnalysis {
namespace {
/// A prompt candidate still inside the coincidence time window.
struct WindowEvent {
  ULong64_t entry;
  ULong64_t time;
  Int_t eventID;
  double x, y, z;
  bool hasFollower;
};

/// The closest-in-time partner of a tagged event, keyed by rdfentry_.
struct PartnerInfo {
  ULong64_t entry;
  Int_t partnerEventID;
  double dt, dr;
};

/// Tag bitmaps are indexed by rdfentry_ and grown on demand.
inline void
MarkEntry(std::vector<bool>& bits, ULong64_t entry) {
  if (entry >= bits.size()) bits.resize(entry + 1, false);
  bits[entry] = true;
}

inline bool
IsMarked(const std::vector<bool>& bits, ULong64_t entry) {
  return entry < bits.size() && bits[entry];
}

/// Partner lists are sparse (one entry per tagged event) and sorted by entry.
inline const PartnerInfo*
FindPartner(const std::vector<PartnerInfo>& partners, ULong64_t entry) {
  auto it = std::lower_bound(partners.begin(), partners.end(), entry,
                             [](const PartnerInfo& p, ULong64_t e) { return p.entry < e; });
  return (it != partners.end() && it->entry == entry) ? &*it : nullptr;
}

constexpr Int_t kNoPartner = -1;
const double kNoValue = std::numeric_limits<double>::quiet_NaN();
} // namespace

void
CoincidenceStep::Configure(const nlohmann::json& config) {
  Step::Configure(config);
  fSequentialOnly = true;
  fTimeWindow = config["time_window"];
  fUseDeltaR = config.contains("delta_r_limit");
  fDeltaRWindow = config.value("delta_r_limit", std::numeric_limits<double>::max());
  fDetail = config.value("detail", false);
  fLabel = config["label"].get<std::string>();
  fPromptTagName = fLabel + "_prompt";
  fDelayedTagName = fLabel + "_follower";
  fPromptExpr = config["prompt_expr"];
  fDelayedExpr = config["delayed_expr"];
  fIdColumn = config.value("id_column", "eventID");
  fPosColumns = config.value("pos_columns", std::array<std::string, 3>{"posx", "posy", "posz"});
  if (fPromptTagName == fDelayedTagName) {
    throw std::runtime_error("Prompt and delayed tag names must be different.");
  }
}

ROOT::RDF::RNode
CoincidenceStep::DoExecute(ROOT::RDF::RNode input) {
  auto start = std::chrono::high_resolution_clock::now();
  auto candidates = DefineColumn(DefineColumn(input, fPromptTagName, fPromptExpr, fRedefine), fDelayedTagName,
                                 fDelayedExpr, fRedefine);

  // Events arrive in entry order, which is time order, so prompt candidates can be held in a
  // deque and dropped once they fall out of the time window. Only the window is ever resident,
  // rather than the whole dataset.
  std::deque<WindowEvent> window;
  std::vector<bool> promptTags, delayedTags;
  // One partner per tagged event rather than one per pair, so these stay O(tagged events).
  std::vector<PartnerInfo> promptPartners, followerPartners;
  size_t nPairs = 0;

  auto evict = [&](ULong64_t now) {
    while (!window.empty() && DeltaT_Clock50(window.front().time, now) > fTimeWindow) {
      window.pop_front();
    }
  };

  // Equivalent to scanning forward from each prompt to the delayed candidates that follow it:
  // here each delayed candidate instead looks back at the prompts still inside the window. The
  // current event joins the window only after being tested, which keeps the "delayed strictly
  // follows prompt" ordering and lets one event be both a prompt and a delayed candidate.
  auto handle = [&](ULong64_t entry, ULong64_t time, Int_t eventID, bool isPrompt, bool isDelayed, bool havePos,
                    double x, double y, double z) {
    evict(time);
    if (isDelayed) {
      bool paired = false;
      Int_t closestID = kNoPartner;
      double closestDt = kNoValue, closestDr = kNoValue;
      for (WindowEvent& prompt : window) {
        double dr = kNoValue;
        if (havePos) {
          const double dX = x - prompt.x;
          const double dY = y - prompt.y;
          const double dZ = z - prompt.z;
          dr = std::sqrt(dX * dX + dY * dY + dZ * dZ);
          if (!(dr < fDeltaRWindow)) continue;
        }
        // qualifying pair found
        const double dt = static_cast<double>(DeltaT_Clock50(prompt.time, time));
        MarkEntry(promptTags, prompt.entry);
        MarkEntry(delayedTags, entry);
        ++nPairs;
        // All followers are later than their prompt, so the first one to pair is the closest.
        if (!prompt.hasFollower) {
          prompt.hasFollower = true;
          promptPartners.push_back({prompt.entry, eventID, dt, dr});
        }
        // The window is time-ordered, so the last qualifying prompt is the closest one.
        paired = true;
        closestID = prompt.eventID;
        closestDt = dt;
        closestDr = dr;
      }
      if (paired) followerPartners.push_back({entry, closestID, closestDt, closestDr});
    }
    if (isPrompt) window.push_back({entry, time, eventID, x, y, z, false});
  };

  // Positions are only read when something needs them: the delta_r cut, or detail's dr.
  if (fUseDeltaR || fDetail) {
    candidates.Foreach([&](ULong64_t entry, ULong64_t time, Int_t eventID, double x, double y, double z, bool isPrompt,
                           bool isDelayed) { handle(entry, time, eventID, isPrompt, isDelayed, true, x, y, z); },
                       {"rdfentry_", "clockCount50", fIdColumn, fPosColumns[0], fPosColumns[1], fPosColumns[2],
                        fPromptTagName, fDelayedTagName});
  } else {
    candidates.Foreach([&](ULong64_t entry, ULong64_t time, Int_t eventID, bool isPrompt,
                           bool isDelayed) { handle(entry, time, eventID, isPrompt, isDelayed, false, 0.0, 0.0, 0.0); },
                       {"rdfentry_", "clockCount50", fIdColumn, fPromptTagName, fDelayedTagName});
  }

  // followerPartners is already ordered: each follower appends once, in entry order. Prompts are
  // filled when their first follower arrives, which need not follow prompt-entry order.
  std::sort(promptPartners.begin(), promptPartners.end(),
            [](const PartnerInfo& a, const PartnerInfo& b) { return a.entry < b.entry; });

  Logger::Debug("Found {} coincidence pairs ({} prompts, {} followers)", nPairs, promptPartners.size(),
                followerPartners.size());
  Logger::Debug(
      "Coincidence step took {} ms",
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count());

  auto result = DefineColumn(
      input, fPromptTagName,
      [pTags = std::make_shared<const std::vector<bool>>(std::move(promptTags))](ULong64_t idx) {
        return IsMarked(*pTags, idx);
      },
      {"rdfentry_"}, fRedefine);
  result = DefineColumn(
      result, fDelayedTagName,
      [dTags = std::make_shared<const std::vector<bool>>(std::move(delayedTags))](ULong64_t idx) {
        return IsMarked(*dTags, idx);
      },
      {"rdfentry_"}, fRedefine);

  // A follower points at its closest prompt, a prompt at its closest follower. These are separate
  // columns because a single event can be both.
  auto prompts = std::make_shared<const std::vector<PartnerInfo>>(std::move(promptPartners));
  auto followers = std::make_shared<const std::vector<PartnerInfo>>(std::move(followerPartners));
  result = DefineColumn(
      result, fLabel + "_prompt_eventID",
      [followers](ULong64_t idx) {
        const PartnerInfo* p = FindPartner(*followers, idx);
        return p ? p->partnerEventID : kNoPartner;
      },
      {"rdfentry_"}, fRedefine);
  result = DefineColumn(
      result, fLabel + "_follower_eventID",
      [prompts](ULong64_t idx) {
        const PartnerInfo* p = FindPartner(*prompts, idx);
        return p ? p->partnerEventID : kNoPartner;
      },
      {"rdfentry_"}, fRedefine);
  if (fDetail) {
    result = DefineColumn(
        result, fLabel + "_prompt_dt",
        [followers](ULong64_t idx) {
          const PartnerInfo* p = FindPartner(*followers, idx);
          return p ? p->dt : kNoValue;
        },
        {"rdfentry_"}, fRedefine);
    result = DefineColumn(
        result, fLabel + "_prompt_dr",
        [followers](ULong64_t idx) {
          const PartnerInfo* p = FindPartner(*followers, idx);
          return p ? p->dr : kNoValue;
        },
        {"rdfentry_"}, fRedefine);
    result = DefineColumn(
        result, fLabel + "_follower_dt",
        [prompts](ULong64_t idx) {
          const PartnerInfo* p = FindPartner(*prompts, idx);
          return p ? p->dt : kNoValue;
        },
        {"rdfentry_"}, fRedefine);
    result = DefineColumn(
        result, fLabel + "_follower_dr",
        [prompts](ULong64_t idx) {
          const PartnerInfo* p = FindPartner(*prompts, idx);
          return p ? p->dr : kNoValue;
        },
        {"rdfentry_"}, fRedefine);
  }
  return result;
}
REGISTER_STEP("coincidence", CoincidenceStep);
} // namespace SnopAnalysis
