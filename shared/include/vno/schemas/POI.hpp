#pragma once
#include "vno/enums/WorldEnums.hpp"
#include <string>
#include <cstdint>

namespace vno {

/// Atomic spec F.1 — POI
struct POI {
    std::string poi_id;
    std::string name_vi;
    std::string name_en;
    double x = 0.0, y = 0.0;
    double lat = 0.0, lng = 0.0;
    POITier tier = POITier::T3;
    POICategory category = POICategory::HISTORY;
    int16_t check_in_radius_m = 50;
    bool requires_photo = false;
    int64_t entry_fee_vnd = 0;
    int32_t exp_reward = 0;
    int32_t first_discovery_bonus = 0;
    bool is_hidden = false;
    std::string description;
    bool checked_in = false;
};

} // namespace vno
