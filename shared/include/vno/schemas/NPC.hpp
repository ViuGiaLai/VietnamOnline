#pragma once
#include "vno/enums/WorldEnums.hpp"
#include <string>
#include <vector>

namespace vno {

struct NPC {
    std::string npc_id;
    std::string name;
    int32_t layer = 2;
    std::string job;
    double x = 0.0, y = 0.0;
    std::string poi_id;
    NPCActivity activity = NPCActivity::WORKING;
    NPCMood mood = NPCMood::NEUTRAL;
    std::vector<std::string> knowledge_tags;
    std::vector<std::string> dialogue_greet;
    std::vector<std::string> dialogue_busy;
};

} // namespace vno
