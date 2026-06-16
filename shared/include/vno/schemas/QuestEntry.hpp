#pragma once
#include "vno/enums/QuestEnums.hpp"
#include <string>
#include <vector>
#include <cstdint>

namespace vno {

struct QuestStep {
    QuestStepType type{};
    std::string description;
    std::string target_id;
    int32_t required = 1;
    int32_t current = 0;
    bool done = false;
};

/// Atomic spec J.1 — QuestEntry
struct QuestEntry {
    std::string quest_id;
    std::string title;
    std::string description;
    QuestType quest_type{};
    QuestStatus status{};
    std::string zone_id;
    std::string giver_npc_id;
    std::vector<QuestStep> steps;
    int64_t reward_vnd = 0;
    int32_t reward_exp = 0;
    int32_t reward_cultural = 0;
    int32_t max_active_same_type = 3;
};

} // namespace vno
