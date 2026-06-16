#pragma once
#include "vno/schemas/POI.hpp"
#include <string>
#include <vector>

namespace vno::osm {

struct OsmNode {
    int64_t osm_id = 0;
    std::string name;
    std::string name_vi;
    std::string street_name;
    std::string amenity;
    std::string tourism;
    double lat = 0.0;
    double lng = 0.0;
};

struct OsmImportResult {
    bool success = false;
    bool from_cache = false;
    int node_count = 0;
    int street_count = 0;
    int enriched_pois = 0;
    std::string error_message;
    std::vector<OsmNode> nodes;
    std::vector<std::string> street_names;
};

/// OpenStreetMap + Overpass (doc FR-06, 3.2 — POI va ten duong that)
class OsmClient {
public:
    static bool configured();
    static OsmImportResult importZone(const std::string& zone_id);
    static int enrichPOIs(std::vector<POI>& pois, const OsmImportResult& osm);
    static std::string nearestStreetName(double lat, double lng, const std::vector<std::string>& streets);
};

} // namespace vno::osm
