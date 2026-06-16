#pragma once
#include "IService.hpp"
#include "vno/config/EnvConfig.hpp"
#include "vno/cache/RedisClient.hpp"
#include "WebSocketServer.hpp"
#include <iostream>
#include <memory>

namespace vno::server {

class GatewayService : public IService {
public:
    std::string name() const override { return "gateway"; }
    void startup() override {
        auto& env = vno::config::EnvConfig::instance();
        std::cout << "[gateway] Listening " << env.get("VNO_GATEWAY_HOST", "0.0.0.0")
                  << ":" << env.getInt("VNO_GATEWAY_PORT", 8080) << " (routing, rate limit)\n";
        if (cache::RedisClient::configured()) {
            std::string msg;
            if (cache::RedisClient::ping(msg))
                std::cout << "[gateway] Redis cache: " << msg << " (session/rate limit)\n";
            else
                std::cout << "[gateway] Redis: " << msg << "\n";
        }

        // Initialize and startup WebSocketServer (FR-04 Multiplayer gateway)
        wsServer_ = std::make_shared<WebSocketServer>();
        wsServer_->startup();
    }
    void shutdown() override {
        if (wsServer_) {
            wsServer_->shutdown();
        }
        std::cout << "[gateway] Stopped\n";
    }

private:
    std::shared_ptr<WebSocketServer> wsServer_;
};

} // namespace vno::server

