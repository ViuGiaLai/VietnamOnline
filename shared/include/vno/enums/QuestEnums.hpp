#pragma once
#include <cstdint>

namespace vno {

enum class QuestType : uint8_t {
    MAIN, SIDE, DAILY, WEEKLY, COMMUNITY, HIDDEN, SEASONAL
};

enum class QuestStatus : uint8_t {
    LOCKED, AVAILABLE, ACTIVE, COMPLETED, FAILED, EXPIRED
};

enum class QuestStepType : uint8_t {
    CHECKIN_POI, TALK_NPC, BUY_FOOD, RENT_VEHICLE, TAKE_PHOTO,
    VISIT_COUNT, TRAVEL_DISTANCE, MEET_PLAYERS
};

enum class ExpRewardType : uint8_t {
    EXPLORER, SOCIAL, CULTURAL, PHOTO, PROFESSION
};

} // namespace vno
