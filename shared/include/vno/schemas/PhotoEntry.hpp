#pragma once
#include <string>
#include <cstdint>

namespace vno {

/// Atomic spec K.1 — PhotoEntry
struct PhotoEntry {
    std::string photo_id;
    std::string poi_id;
    int32_t elo_score = 1000;
    float technical_score = 70.0f;
    int64_t timestamp_ms = 0;
};

} // namespace vno
