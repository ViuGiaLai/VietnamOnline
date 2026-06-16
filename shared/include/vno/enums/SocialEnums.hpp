#pragma once
#include <cstdint>

namespace vno {

enum class ChatChannel : uint8_t {
    PROXIMITY, PARTY, ZONE, WHISPER, GUILD
};

enum class ReputationBadge : uint8_t { NONE, TRUSTED, EXCELLENT };

} // namespace vno
