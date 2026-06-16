#pragma once
#include <cstdint>

namespace vno::constants {

// Currency (G.1)
constexpr int64_t VND_HARD_CAP = 500'000'000LL;
constexpr int64_t DAILY_P2P_CAP = 10'000'000LL;
constexpr float P2P_TAX_RATE = 0.05f;
constexpr int64_t PHOTO_PRICE_FLOOR = 1'000LL;
constexpr int64_t PHOTO_PRICE_CAP = 5'000'000LL;
constexpr int64_t TOUR_PRICE_FLOOR = 50'000LL;
constexpr int64_t TOUR_PRICE_CAP = 10'000'000LL;

// Vehicle (D.1 / D.3)
constexpr int64_t FUEL_REFILL_COST = 15'000LL;
constexpr float FUEL_IDLE_PER_MIN = 0.1f;

// Daily login (R.3)
constexpr int64_t DAILY_LOGIN_BASE = 100'000LL;

} // namespace vno::constants
