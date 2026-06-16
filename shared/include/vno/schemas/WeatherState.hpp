#pragma once
#include "vno/enums/WorldEnums.hpp"
#include <string>
#include <cstdint>

namespace vno {

/// Atomic spec L.1 — WeatherState
struct WeatherState {
    std::string zone_id;
    WeatherType weather_type = WeatherType::SUNNY_CLEAR;
    float temperature_c = 32.0f;
    int32_t visibility_m = 9999;
    float wind_speed_kmh = 5.0f;
    float rain_intensity = 0.0f;
    int64_t updated_at_ms = 0;
    bool override_active = false;
};

} // namespace vno
