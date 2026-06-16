#include "vno/osm/OsmClient.hpp"
#include "vno/config/EnvConfig.hpp"
#include "vno/cache/RedisClient.hpp"
#include "vno/net/HttpClient.hpp"
#include "vno/util/JsonMini.hpp"
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <filesystem>

namespace vno::osm {

namespace {

double haversineM(double lat1, double lng1, double lat2, double lng2) {
    constexpr double R = 6371000.0;
    auto rad = [](double d) { return d * 3.141592653589793 / 180.0; };
    double dlat = rad(lat2 - lat1);
    double dlng = rad(lng2 - lng1);
    double a = std::sin(dlat / 2) * std::sin(dlat / 2) +
               std::cos(rad(lat1)) * std::cos(rad(lat2)) * std::sin(dlng / 2) * std::sin(dlng / 2);
    return R * 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
}

std::string cachePath(const std::string& zone_id) {
    auto& env = vno::config::EnvConfig::instance();
    std::string dir = env.get("MAP_TILE_CACHE_DIR", "data/cache/tiles");
    std::filesystem::create_directories(dir + "/osm");
    return dir + "/osm/" + zone_id + "_overpass.json";
}

std::string overpassQueryHoiAn() {
    return R"([out:json][timeout:30];
(
  node["name"](15.85,108.30,15.92,108.36);
  way["highway"]["name"](15.85,108.30,15.92,108.36);
  node["tourism"](15.85,108.30,15.92,108.36);
  node["amenity"~"restaurant|cafe|place_of_worship|museum"](15.85,108.30,15.92,108.36);
);
out body;)";
}

bool loadCache(const std::string& path, std::string& json) {
    std::ifstream f(path);
    if (!f) return false;
    std::ostringstream ss;
    ss << f.rdbuf();
    json = ss.str();
    return json.size() > 50;
}

bool saveCache(const std::string& path, const std::string& json) {
    std::ofstream f(path);
    if (!f) return false;
    f << json;
    return true;
}

void parseOverpassJson(const std::string& json, OsmImportResult& out) {
    size_t pos = 0;
    while ((pos = json.find("\"type\":\"node\"", pos)) != std::string::npos) {
        size_t block_end = json.find("\"type\":", pos + 10);
        if (block_end == std::string::npos) block_end = json.size();
        std::string block = json.substr(pos, block_end - pos);

        OsmNode node;
        node.osm_id = static_cast<int64_t>(util::jsonExtractNumber(block, "id"));
        node.lat = util::jsonExtractNumber(block, "lat");
        node.lng = util::jsonExtractNumber(block, "lon");
        node.name = util::jsonExtractString(block, "name");
        node.name_vi = util::jsonExtractString(block, "name:vi");
        node.amenity = util::jsonExtractString(block, "amenity");
        node.tourism = util::jsonExtractString(block, "tourism");

        if (node.lat != 0.0 && node.lng != 0.0 && !node.name.empty()) {
            out.nodes.push_back(node);
        }
        pos = block_end;
    }

  // Street names from ways
    pos = 0;
    while ((pos = json.find("\"type\":\"way\"", pos)) != std::string::npos) {
        size_t block_end = json.find("\"type\":", pos + 10);
        if (block_end == std::string::npos) block_end = std::min(pos + 800, json.size());
        std::string block = json.substr(pos, block_end - pos);
        auto name = util::jsonExtractString(block, "name");
        if (!name.empty()) {
            if (std::find(out.street_names.begin(), out.street_names.end(), name) == out.street_names.end())
                out.street_names.push_back(name);
        }
        pos = block_end;
    }

    out.node_count = static_cast<int>(out.nodes.size());
    out.street_count = static_cast<int>(out.street_names.size());
}

} // namespace

bool OsmClient::configured() {
    return !vno::config::EnvConfig::instance().get("OSM_OVERPASS_URL", "").empty();
}

OsmImportResult OsmClient::importZone(const std::string& zone_id) {
    OsmImportResult result;
    if (!configured()) {
        result.error_message = "OSM_OVERPASS_URL chua cau hinh";
        return result;
    }

    std::string cache_file = cachePath(zone_id);
    std::string json;

    if (loadCache(cache_file, json)) {
        result.from_cache = true;
    } else {
        auto& env = vno::config::EnvConfig::instance();
        std::string url = env.get("OSM_OVERPASS_URL", "https://overpass-api.de/api/interpreter");
        std::string query = overpassQueryHoiAn();
        std::vector<net::HttpHeader> headers = {{"Content-Type", "text/plain"}};

        auto resp = net::httpRequest("POST", url, headers, query);
        if (!resp.ok()) {
            result.error_message = "Overpass HTTP " + std::to_string(resp.status);
            return result;
        }
        json = resp.body;
        saveCache(cache_file, json);

        if (cache::RedisClient::configured()) {
            cache::RedisClient::set("vno:osm:" + zone_id + ":count",
                std::to_string(static_cast<int>(json.size())), 86400 * 7);
        }
    }

    parseOverpassJson(json, result);
    result.success = result.node_count > 0 || result.street_count > 0;
    if (!result.success) result.error_message = "Khong parse duoc du lieu OSM";
    return result;
}

int OsmClient::enrichPOIs(std::vector<POI>& pois, const OsmImportResult& osm) {
    int enriched = 0;
    for (auto& poi : pois) {
        if (poi.lat == 0.0 && poi.lng == 0.0) continue;
        const OsmNode* best = nullptr;
        double best_d = 250.0;
        for (const auto& node : osm.nodes) {
            double d = haversineM(poi.lat, poi.lng, node.lat, node.lng);
            if (d < best_d) {
                best_d = d;
                best = &node;
            }
        }
        if (best) {
            if (!best->name_vi.empty()) poi.name_vi = best->name_vi;
            else if (!best->name.empty() && poi.name_vi.find(best->name) == std::string::npos)
                poi.description += " [OSM: " + best->name + "]";
            enriched++;
        }
    }
    return enriched;
}

std::string OsmClient::nearestStreetName(double lat, double lng, const std::vector<std::string>& streets) {
    if (streets.empty()) return "";
    return streets[static_cast<size_t>(std::fmod(lat * 1000 + lng, static_cast<double>(streets.size())))];
}

} // namespace vno::osm
