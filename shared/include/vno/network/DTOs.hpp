#pragma once
#include "vno/network/Protocol.hpp"

namespace vno::network {

struct ApiResponse {
    bool success = false;
    int32_t code = 0;
    std::string message;
};

struct AuthLoginResponse : ApiResponse {
    std::string access_token;
    std::string refresh_token;
    std::string player_id;
};

struct ProfileSnapshot : ApiResponse {
    std::string player_id;
    std::string display_name;
    std::string zone_id;
    int64_t vnd_balance = 0;
};

} // namespace vno::network
