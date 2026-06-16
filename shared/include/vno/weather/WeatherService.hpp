#pragma once
#include "vno/schemas/WeatherState.hpp"
#include <string>

namespace vno::weather {

/// OpenWeatherMap sync (doc L.1 — cap nhat moi 15 phut, gameplay effects)
class WeatherService {
public:
    static bool configured();
    static bool fetchForZone(const std::string& zone_id, WeatherState& out);
    static WeatherType mapOpenWeatherMain(const std::string& main, int weather_id, float temp_c);
};

} // namespace vno::weather
