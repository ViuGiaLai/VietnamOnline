#pragma once
#include <cstdint>

namespace vno {

enum class POICategory : uint8_t {
    HISTORY, NATURE, SPIRITUAL, FOOD, ENTERTAINMENT, HERITAGE
};

enum class POITier : uint8_t { T1 = 1, T2 = 2, T3 = 3, T4 = 4 };

enum class WeatherType : uint8_t {
    SUNNY_CLEAR, SUNNY_HOT, RAIN_LIGHT, RAIN_HEAVY, STORM, FOG, CLOUDY
};

enum class NPCActivity : uint8_t { WORKING, RESTING, EATING, COMMUTING, SLEEPING };
enum class NPCMood : uint8_t { HAPPY, NEUTRAL, BUSY, TIRED };

inline const char* weatherName(WeatherType w) {
    switch (w) {
        case WeatherType::SUNNY_CLEAR: return "Nang dep";
        case WeatherType::SUNNY_HOT: return "Nang nong";
        case WeatherType::RAIN_LIGHT: return "Mua nhe";
        case WeatherType::RAIN_HEAVY: return "Mua to";
        case WeatherType::STORM: return "Bao";
        case WeatherType::FOG: return "Suong mu";
        case WeatherType::CLOUDY: return "Nhieu may";
        default: return "Khong ro";
    }
}

} // namespace vno
