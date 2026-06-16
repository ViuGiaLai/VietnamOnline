#pragma once
#include "vno/schemas/PlayerEntity.hpp"
#include "vno/constants/SyncConstants.hpp"
#include <cmath>

namespace vno::validation {

/// IV-01, IV-02 — movement anti-cheat
inline bool isSpeedValid(double distance_m, float max_speed_ms, float delta_sec) {
    if (delta_sec <= 0.0f) return true;
    double max_allowed = max_speed_ms * constants::MAX_SPEED_HACK_MULTIPLIER * delta_sec;
    return distance_m <= max_allowed;
}

inline bool isTeleportSuspect(double distance_m, float delta_sec) {
    if (delta_sec <= 0.0f) return false;
    return (distance_m / delta_sec) > constants::MAX_TELEPORT_M_PER_SEC;
}

/// F.3 — POI check-in distance
inline bool isWithinCheckInRadius(double dist_m, int16_t radius_m) {
    return dist_m <= static_cast<double>(radius_m);
}

/// G.1 — wallet cap
inline bool canAfford(int64_t balance, int64_t amount) {
    return balance >= amount && amount >= 0;
}

/// J — active quest cap
inline bool canAcceptQuest(std::size_t active_count) {
    return active_count < static_cast<std::size_t>(constants::MAX_ACTIVE_QUESTS);
}

/// E — buff stack cap
inline bool canAddBuff(std::size_t buff_count) {
    return buff_count < static_cast<std::size_t>(constants::MAX_ACTIVE_BUFFS);
}

} // namespace vno::validation
