#include "../common/include/ServiceRegistry.hpp"
#include "../gateway/include/GatewayService.hpp"
#include "../auth-service/include/AuthService.hpp"
#include "../profile-service/include/ProfileService.hpp"
#include "../world-service/include/WorldService.hpp"
#include "../movement-sync-service/include/MovementSyncService.hpp"
#include "../chat-service/include/ChatService.hpp"
#include "../poi-service/include/PoiService.hpp"
#include "../quest-service/include/QuestService.hpp"
#include "../vehicle-service/include/VehicleService.hpp"
#include "vno/config/EnvConfig.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>


int main() {
    auto& env = vno::config::EnvConfig::instance();
    env.load(".env");

    std::cout << "==========================================\n";
    std::cout << "  VIETNAM ONLINE - MVP Server ("
              << env.get("VNO_DEFAULT_ZONE", "HOI_AN") << ")\n";
    std::cout << "  Env: " << env.get("VNO_APP_ENV", "development") << "\n";
    std::cout << "==========================================\n";

    vno::server::ServiceRegistry registry;
    registry.registerService(std::make_shared<vno::server::GatewayService>());
    registry.registerService(std::make_shared<vno::server::AuthService>());
    registry.registerService(std::make_shared<vno::server::ProfileService>());
    registry.registerService(std::make_shared<vno::server::WorldService>());
    registry.registerService(std::make_shared<vno::server::MovementSyncService>());
    registry.registerService(std::make_shared<vno::server::ChatService>());
    registry.registerService(std::make_shared<vno::server::PoiService>());
    registry.registerService(std::make_shared<vno::server::QuestService>());
    registry.registerService(std::make_shared<vno::server::VehicleService>());

    registry.startupAll();
    std::cout << "\nServer ready. MVP services: 9/9 (Phase 1)\n";
    std::cout << "Future: zone-instance, social, economy, npc, photo, analytics, moderation, admin-gm\n";
    std::cout << "==========================================\n";
    std::cout << "Server is running. Type 'exit' and press Enter to stop.\n";
    
    std::string cmd;
    while (std::getline(std::cin, cmd)) {
        if (cmd == "exit") {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    registry.shutdownAll();
    return 0;
}

