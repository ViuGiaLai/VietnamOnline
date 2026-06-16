#pragma once
#include "IService.hpp"
#include "vno/schemas/WeatherState.hpp"
#include "vno/config/EnvConfig.hpp"
#include "vno/weather/WeatherService.hpp"
#include <iostream>

namespace vno::server {

class WorldService : public IService {
public:
    std::string name() const override { return "world-service"; }
    void startup() override {
        auto& env = vno::config::EnvConfig::instance();
        weather_.zone_id = env.get("VNO_DEFAULT_ZONE", "HOI_AN");
        if (weather::WeatherService::configured()) {
            if (weather::WeatherService::fetchForZone(weather_.zone_id, weather_))
                std::cout << "[world-service] OpenWeatherMap: " << weatherName(weather_.weather_type)
                          << " " << (int)weather_.temperature_c << "C\n";
            else
                std::cout << "[world-service] Weather API loi — dung mac dinh\n";
        } else {
            weather_.weather_type = WeatherType::SUNNY_CLEAR;
            std::cout << "[world-service] Weather simulated (chua co OPENWEATHERMAP_API_KEY)\n";
        }
        std::cout << "[world-service] Zone " << weather_.zone_id << " sync "
                  << env.getInt("WEATHER_SYNC_INTERVAL_MIN", 15) << "min\n";
    }
    void shutdown() override {}
    const WeatherState& weather() const { return weather_; }

private:
    WeatherState weather_;
};

} // namespace vno::server
