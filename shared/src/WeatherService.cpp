#include "vno/weather/WeatherService.hpp"
#include "vno/config/EnvConfig.hpp"
#include "vno/cache/RedisClient.hpp"
#include "vno/net/HttpClient.hpp"
#include "vno/util/JsonMini.hpp"
#include <sstream>
#include <cmath>
#include <chrono>

namespace vno::weather {

namespace {

struct ZoneCoords {
    double lat;
    double lng;
};

ZoneCoords zoneCoords(const std::string& zone_id) {
    auto& env = vno::config::EnvConfig::instance();
    if (zone_id == "HOI_AN") {
        return {
            env.getDouble("VNO_ZONE_HOI_AN_LAT", 15.8794),
            env.getDouble("VNO_ZONE_HOI_AN_LNG", 108.3294)
        };
    }
    return {15.8794, 108.3294};
}

std::string cacheKey(const std::string& zone_id) {
    return "vno:weather:" + zone_id;
}

bool parseWeatherJson(const std::string& json, WeatherState& out) {
    float temp = static_cast<float>(util::jsonExtractNumber(json, "temp"));
    if (temp == 0.0f) {
        auto main_pos = json.find("\"main\"");
        if (main_pos != std::string::npos) {
            std::string block = json.substr(main_pos, 200);
            temp = static_cast<float>(util::jsonExtractNumber(block, "temp"));
        }
    }
    out.temperature_c = temp;

    auto weather_pos = json.find("\"weather\"");
    std::string main = "Clear";
    int wid = 800;
    if (weather_pos != std::string::npos) {
        std::string block = json.substr(weather_pos, 300);
        main = util::jsonExtractString(block, "main");
        if (main.empty()) main = "Clear";
        wid = static_cast<int>(util::jsonExtractNumber(block, "id"));
    }

    auto wind_pos = json.find("\"wind\"");
    if (wind_pos != std::string::npos) {
        std::string block = json.substr(wind_pos, 120);
        out.wind_speed_kmh = static_cast<float>(util::jsonExtractNumber(block, "speed")) * 3.6f;
    }

    out.weather_type = WeatherService::mapOpenWeatherMain(main, wid, temp);
    out.rain_intensity = (out.weather_type == WeatherType::RAIN_LIGHT) ? 0.3f :
                         (out.weather_type == WeatherType::RAIN_HEAVY || out.weather_type == WeatherType::STORM) ? 0.8f : 0.0f;
    out.visibility_m = (out.weather_type == WeatherType::FOG) ? 500 : 9999;
    out.updated_at_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return temp != 0.0f || !main.empty();
}

} // namespace

bool WeatherService::configured() {
    return !vno::config::EnvConfig::instance().get("OPENWEATHERMAP_API_KEY", "").empty();
}

WeatherType WeatherService::mapOpenWeatherMain(const std::string& main, int weather_id, float temp_c) {
    if (main == "Thunderstorm" || weather_id >= 200 && weather_id < 300)
        return WeatherType::STORM;
    if (main == "Drizzle" || (main == "Rain" && weather_id < 522))
        return WeatherType::RAIN_LIGHT;
    if (main == "Rain")
        return WeatherType::RAIN_HEAVY;
    if (main == "Fog" || main == "Mist" || main == "Haze")
        return WeatherType::FOG;
    if (main == "Clouds")
        return WeatherType::CLOUDY;
    if (main == "Clear" && temp_c >= 34.0f)
        return WeatherType::SUNNY_HOT;
    return WeatherType::SUNNY_CLEAR;
}

bool WeatherService::fetchForZone(const std::string& zone_id, WeatherState& out) {
    out.zone_id = zone_id;
    auto& env = vno::config::EnvConfig::instance();
    int ttl = env.getInt("WEATHER_SYNC_INTERVAL_MIN", 15) * 60;

    if (cache::RedisClient::configured()) {
        if (auto cached = cache::RedisClient::get(cacheKey(zone_id))) {
            if (parseWeatherJson(*cached, out)) {
                out.zone_id = zone_id;
                return true;
            }
        }
    }

    std::string api_key = env.get("OPENWEATHERMAP_API_KEY", "");
    if (api_key.empty()) return false;

    auto coords = zoneCoords(zone_id);
    std::ostringstream url;
    url << "https://api.openweathermap.org/data/2.5/weather?lat=" << coords.lat
        << "&lon=" << coords.lng << "&appid=" << api_key << "&units=metric&lang=vi";

    auto resp = net::httpRequest("GET", url.str(), {});
    if (!resp.ok()) return false;
    if (!parseWeatherJson(resp.body, out)) return false;
    out.zone_id = zone_id;

    if (cache::RedisClient::configured())
        cache::RedisClient::set(cacheKey(zone_id), resp.body, ttl);

    return true;
}

} // namespace vno::weather
