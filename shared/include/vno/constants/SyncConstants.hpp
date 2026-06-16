#pragma once
#include <cstdint>

namespace vno::constants {

// ServerSync (N.1)
constexpr int32_t GAME_SERVER_TICK_HZ = 20;
constexpr int32_t WORLD_STATE_TICK_HZ = 5;
constexpr int32_t POSITION_SYNC_INTERVAL_MS = 50;
constexpr int32_t MAX_CCU_PER_ZONE = 200;
constexpr int32_t CHECKPOINT_SAVE_INTERVAL_SEC = 30;
constexpr int32_t RECONNECT_GRACE_PERIOD_SEC = 60;
constexpr int32_t IDLE_ZONE_SLEEP_TIMEOUT_SEC = 300;
constexpr int32_t DEAD_RECKONING_WINDOW_MS = 200;

// Anti-cheat (B.2 / IV rules)
constexpr float MAX_SPEED_HACK_MULTIPLIER = 1.5f;
constexpr double MAX_TELEPORT_M_PER_SEC = 500.0;
constexpr int32_t MAX_INPUT_ACTIONS_PER_SEC = 120;

} // namespace vno::constants
