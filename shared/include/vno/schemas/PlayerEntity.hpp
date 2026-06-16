#pragma once
#include "vno/enums/PlayerEnums.hpp"
#include "vno/schemas/BuffEntry.hpp"
#include "vno/schemas/PhotoEntry.hpp"
#include <string>
#include <vector>
#include <set>
#include <optional>
#include <cstdint>

namespace vno {

struct VehicleRef {
    VehicleType type{};
    std::string model;
    float max_speed_kmh = 0.0f;
    float fuel_km_remaining = 0.0f;
    float fuel_tank_km = 0.0f;
    int64_t rental_cost_hour = 0;
    int64_t rental_start_time = 0;
};

/// Atomic spec A.1 — PlayerEntity
struct PlayerEntity {
    std::string player_id;
    std::string display_name;
    std::string gender = "nam";
    std::string skin_tone = "medium";

    double pos_x = 0.0, pos_y = 0.0, pos_z = 0.0;
    std::string zone_id = "HOI_AN";

    float stamina_cur = 100.0f;
    float stamina_max = 100.0f;
    int64_t vnd_balance = 0;
    int32_t xu_vang = 0;
    int16_t explorer_lv = 1;
    int16_t cultural_knowledge = 0;
    float reputation_score = 3.0f;
    int32_t photo_score_elo = 1000;

    MovementState state = MovementState::IDLE;
    std::optional<VehicleRef> active_vehicle;
    std::vector<BuffEntry> active_buffs;
    std::set<std::string> checked_in_pois;
    std::vector<std::string> active_quest_ids;
    std::vector<PhotoEntry> photos;
    std::vector<std::string> met_npc_ids;

    bool onboarding_complete = false;
    int onboarding_step = 0;
    int login_streak = 0;
    float total_distance_km = 0.0f;
    int game_hour = 8;
    int game_minute = 0;
};

struct OnlinePlayer {
    std::string player_id;
    std::string display_name;
    double pos_x = 0.0, pos_y = 0.0;
    std::string zone_id;
};

} // namespace vno
