#pragma once
#include "vno/schemas/POI.hpp"
#include <string>

namespace vno::validation {

inline bool isPoiOpen(int game_hour, int /*game_minute*/) {
    // MVP: pho co mo 6h-22h
    return game_hour >= 6 && game_hour < 22;
}

inline std::string checkInFailMessage(double dist_m, int16_t radius_m) {
    return "Den gan hon de check-in! Con " + std::to_string(static_cast<int>(dist_m))
         + "m (can < " + std::to_string(radius_m) + "m)";
}

} // namespace vno::validation
