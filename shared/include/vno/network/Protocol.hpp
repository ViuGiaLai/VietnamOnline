#pragma once
#include <cstdint>
#include <string>

namespace vno::network {

enum class Protocol : uint8_t {
    UDP_POSITION,
    WEBSOCKET_CHAT,
    HTTPS_REST,
    HTTPS_SSE_NPC,
    HTTP_CDN
};

/// Player position sync packet (N.2)
struct PlayerPositionPacket {
    std::string player_id;
    double pos_x, pos_y, pos_z;
    float rotation_yaw;
    int64_t client_timestamp_ms;
    int64_t server_timestamp_ms;
};

struct ChatSendRequest {
    std::string channel;
    std::string content;
};

struct CheckInRequest {
    std::string poi_id;
    std::string photo_id; // optional
};

struct QuestAcceptRequest {
    std::string quest_id;
};

struct VehicleRentRequest {
    std::string station_id;
    std::string vehicle_type;
};

} // namespace vno::network
