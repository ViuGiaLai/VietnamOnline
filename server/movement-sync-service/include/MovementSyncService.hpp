#pragma once
#include "IService.hpp"
#include "vno/config/EnvConfig.hpp"
#include "vno/cache/RedisClient.hpp"
#include <iostream>

namespace vno::server {

class MovementSyncService : public IService {
public:
    std::string name() const override { return "movement-sync-service"; }
    void startup() override {
        auto& env = vno::config::EnvConfig::instance();
        std::cout << "[movement-sync] tick=" << env.getInt("VNO_GAME_SERVER_TICK_HZ", 20)
                  << "Hz sync=" << env.getInt("VNO_POSITION_SYNC_MS", 50)
                  << "ms max_ccu=" << env.getInt("VNO_MAX_CCU_PER_ZONE", 200) << "\n";
        if (cache::RedisClient::configured()) {
            std::string zone = env.get("VNO_DEFAULT_ZONE", "HOI_AN");
            cache::RedisClient::set("vno:zone:" + zone + ":ccu", "0", 3600);
            std::cout << "[movement-sync] Redis session key vno:zone:" << zone << ":ccu\n";
        }
    }
    void shutdown() override {}
};

} // namespace vno::server
