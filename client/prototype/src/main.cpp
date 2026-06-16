#include "vno/config/EnvConfig.hpp"
#include "AuthManager.hpp"
#include "GameEngine.hpp"
#include <iostream>
#include <string>

static void printBanner() {
    std::cout << R"(
  __     ___         _             _    ___       _       _ 
  \ \   / (_)_ __   / \   _ __   __| |  / _ \ _ __ | | __ _| |
   \ \ / /| | '_ \ / _ \ | '_ \ / _` | | | | | '_ \| |/ _` | |
    \ V / | | | | / ___ \| | | | (_| | | |_| | | | | | (_| | |
     \_/  |_|_| |_/_/   \_\_| |_|\__,_|  \___/|_| |_|_|\__,_|_|
)" << "\n";
    std::cout << "     VIETNAM ONLINE - MVP Hoi An (theo tai lieu thiet ke v1.0)\n";
    std::cout << "     Virtual Cultural Tourism MMO - Phien ban Core C++\n";
    std::cout << "============================================================\n\n";
}

int main() {
    printBanner();

    auto& env = vno::config::EnvConfig::instance();
    env.load(".env");
    std::cout << "Zone: " << env.get("VNO_DEFAULT_ZONE", "HOI_AN")
              << " | API: " << env.get("VNO_API_BASE_URL", "http://localhost:8080") << "\n\n";

    std::string player_id;
    if (!vno::client::AuthManager::devSkipEnabled()) {
        std::cout << "=== DANG NHAP (FR-01) ===\n";
        std::cout << "1. Dang ky\n2. Dang nhap\nChon: ";
        std::string auth_choice;
        std::getline(std::cin, auth_choice);
        std::string email, password;
        std::cout << "Email: "; std::getline(std::cin, email);
        std::cout << "Mat khau: "; std::getline(std::cin, password);
        bool ok = false;
        if (auth_choice == "1") ok = vno::client::AuthManager::registerAccount(email, password, player_id);
        else ok = vno::client::AuthManager::login(email, password, player_id);
        if (!ok) { std::cout << "Auth that bai!\n"; return 1; }
        std::cout << "Xac thuc thanh cong! Player ID: " << player_id << "\n\n";
    }

    GameEngine game;
    if (!player_id.empty()) game.player().player_id = player_id;

    bool cloud_available = vno::client::AuthManager::usingSupabase()
        && !vno::client::AuthManager::accessToken().empty();

    std::cout << "1. Bat dau game moi (Onboarding FTUE)\n";
    std::cout << "2. Tai game da luu (local)\n";
    if (cloud_available) std::cout << "3. Tai tu Supabase (cloud)\n";
    std::cout << "Chon: ";
    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "3" && cloud_available) {
        if (game.loadFromCloud()) {
            game.applyDailyLoginReward();
        } else {
            std::cout << "Chua co save cloud. Bat dau moi...\n";
            game.runCharacterCreator();
            game.runOnboarding();
            game.applyDailyLoginReward();
        }
    } else if (choice == "2" && game.hasSave()) {
        if (game.loadGame()) {
            std::cout << "Tai game thanh cong!\n";
            game.applyDailyLoginReward();
        } else {
            std::cout << "Loi tai game. Bat dau moi...\n";
            game.runCharacterCreator();
            game.runOnboarding();
            game.applyDailyLoginReward();
        }
    } else {
        game.runCharacterCreator();
        game.runOnboarding();
        game.applyDailyLoginReward();
    }

    game.processCommand("quest accept QUEST-MAIN-01");

    std::cout << "\nBan xuat phat gan Cho Dem Hoi An. Go 'help' de xem lenh.\n";
    std::cout << "MUC TIEU: Di den Chua Cau (100,100) - go 'go 100 100' hoac dung WASD.\n\n";

    game.showHUD();

    while (game.isRunning()) {
        game.tickRetention();
        std::cout << "\n> ";
        std::string input;
        if (!std::getline(std::cin, input)) break;
        game.processCommand(input);
        if (game.isRunning()) game.showHUD();
    }

    return 0;
}
