#pragma once
#include <cstdint>

namespace vno::constants {

// PlayerEntity (A.1)
constexpr float STAMINA_DEFAULT = 100.0f;
constexpr float STAMINA_MAX_CAP = 150.0f;
constexpr int64_t VND_MAX_BALANCE = 500'000'000LL;
constexpr int32_t MAX_ACTIVE_BUFFS = 5;
constexpr int32_t MAX_ACTIVE_QUESTS = 3;
constexpr int32_t DISPLAY_NAME_MAX = 32;

// Movement (A.3)
constexpr float WALK_SPEED_MS = 1.39f;      // 5 km/h
constexpr float SPRINT_SPEED_MS = 2.50f;    // 9 km/h
constexpr float WALK_HOT_SPEED_MS = 1.11f;  // -20% nang nong
constexpr float WALK_EXHAUSTED_MS = 0.69f;  // stamina = 0

// Stamina (C.1)
constexpr float STM_IDLE = 2.0f;
constexpr float STM_SPRINT = -1.0f;
constexpr float STM_SITTING = 5.0f;
constexpr float STM_HOT_PENALTY = -0.5f;

// Social
constexpr double PROXIMITY_CHAT_RADIUS_M = 50.0;
constexpr double INTEREST_MGMT_RADIUS_M = 200.0;

// POI check-in radius by tier (F.1)
constexpr int16_t POI_RADIUS_T1 = 100;
constexpr int16_t POI_RADIUS_T2 = 75;
constexpr int16_t POI_RADIUS_T3 = 50;
constexpr int16_t POI_RADIUS_T4 = 30;

} // namespace vno::constants
