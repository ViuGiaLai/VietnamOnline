#pragma once
#include "IService.hpp"
#include "vno/config/EnvConfig.hpp"
#include <iostream>

namespace vno::server {

class ChatService : public IService {
public:
    std::string name() const override { return "chat-service"; }
    void startup() override {
        auto& env = vno::config::EnvConfig::instance();
        std::cout << "[chat-service] Proximity " << env.getDouble("VNO_PROXIMITY_CHAT_RADIUS_M", 50.0)
                  << "m, expiry 24h (FR-05)\n";
    }
    void shutdown() override {}
};

} // namespace vno::server
