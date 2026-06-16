#pragma once
#include "vno/enums/SocialEnums.hpp"
#include <string>
#include <cstdint>

namespace vno {

/// Atomic spec I.1 — ChatMessage
struct ChatMessage {
    std::string msg_id;
    ChatChannel channel{};
    std::string sender_id;
    std::string sender_name;
    std::string content;
    int64_t timestamp_ms = 0;
    int32_t expiry_seconds = 86400;
};

} // namespace vno
