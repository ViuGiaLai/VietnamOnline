#pragma once
#include "vno/schemas/PlayerEntity.hpp"
#include "vno/enums/PlayerEnums.hpp"
#include <iostream>

namespace vno::client {

inline void displayPlayerStatus(const PlayerEntity& player) {
    std::cout << "\n========== " << player.display_name << " [" << player.player_id << "] ==========\n";
    std::cout << "Zone: " << player.zone_id << " | Vi tri: (" << (int)player.pos_x << ", " << (int)player.pos_y << ")\n";
    std::cout << "Gio game: " << player.game_hour << ":" << (player.game_minute < 10 ? "0" : "") << player.game_minute << "\n";
    std::cout << "Stamina: " << (int)player.stamina_cur << "/" << (int)player.stamina_max;
    if (player.stamina_cur <= 19) std::cout << " [MET!]";
    std::cout << "\n";
    std::cout << "VND: " << player.vnd_balance << " | Xu Vang: " << player.xu_vang << "\n";
    std::cout << "Explorer Lv." << player.explorer_lv << " | Van hoa: " << player.cultural_knowledge
              << " | Uy tin: " << player.reputation_score << " sao\n";
    std::cout << "Photo ELO: " << player.photo_score_elo << " | Check-in: " << player.checked_in_pois.size() << " dia diem\n";
    std::cout << "Trang thai: " << movementStateName(player.state);
    if (player.active_vehicle) std::cout << " | Xe: " << player.active_vehicle->model;
    std::cout << "\n";
    if (!player.active_buffs.empty()) {
        std::cout << "Buff: ";
        for (const auto& b : player.active_buffs) std::cout << b.source_food << "(" << b.remaining_seconds << "s) ";
        std::cout << "\n";
    }
    std::cout << "================================================\n";
}

} // namespace vno::client
