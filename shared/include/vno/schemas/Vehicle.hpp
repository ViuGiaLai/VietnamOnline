#pragma once
#include "vno/enums/PlayerEnums.hpp"
#include "vno/schemas/BuffEntry.hpp"
#include <string>
#include <vector>
#include <cstdint>

namespace vno {

struct FoodItem {
    std::string name;
    std::string region;
    int64_t price = 0;
    BuffEntry buff;
    bool eat_while_moving = false;
};

struct VehicleConfig {
    VehicleType type{};
    std::string model;
    float max_speed_kmh = 0.0f;
    float fuel_tank_km = 0.0f;
    int64_t rental_cost_hour = 0;
    bool alley_access = true;
    int32_t unlock_phase = 1;
};

struct RentalStation {
    std::string station_id;
    std::string name;
    double x = 0.0, y = 0.0;
    std::vector<VehicleType> available;
};

} // namespace vno
