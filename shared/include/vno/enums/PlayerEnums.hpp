#pragma once
#include <cstdint>

namespace vno {

enum class MovementState : uint8_t {
    IDLE, WALKING, SPRINTING, RIDING_VEHICLE, PASSENGER,
    INTERACTING, PHOTO_MODE, SITTING_REST, EATING
};

enum class BuffType : uint8_t {
    STAMINA_REGEN, SPEED_BOOST, EXP_BONUS, PRICE_DISCOUNT,
    PHOTO_BONUS, SOCIAL_BONUS, ANTI_NAUSEA
};

enum class StackBehavior : uint8_t { REFRESH, NO_STACK };

enum class VehicleType : uint8_t {
    BICYCLE, HONDA_WAVE, YAMAHA_EXCITER, HONDA_SH, XICH_LO
};

inline const char* movementStateName(MovementState s) {
    switch (s) {
        case MovementState::IDLE: return "IDLE";
        case MovementState::WALKING: return "WALKING";
        case MovementState::SPRINTING: return "SPRINTING";
        case MovementState::RIDING_VEHICLE: return "RIDING_VEHICLE";
        case MovementState::PASSENGER: return "PASSENGER";
        case MovementState::INTERACTING: return "INTERACTING";
        case MovementState::PHOTO_MODE: return "PHOTO_MODE";
        case MovementState::SITTING_REST: return "SITTING_REST";
        case MovementState::EATING: return "EATING";
        default: return "UNKNOWN";
    }
}

} // namespace vno
