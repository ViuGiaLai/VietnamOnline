#pragma once
#include "vno/enums/PlayerEnums.hpp"
#include <string>
#include <vector>

namespace vno {

struct BuffEntry {
    std::string buff_id;
    BuffType buff_type{};
    std::string source_food;
    float value_primary = 0.0f;
    float value_secondary = 0.0f;
    int32_t duration_seconds = 0;
    int32_t remaining_seconds = 0;
    StackBehavior stack_behavior = StackBehavior::REFRESH;
    std::vector<std::string> region_lock;
};

} // namespace vno
