#pragma once
#include "IService.hpp"
#include "vno/config/EnvConfig.hpp"
#include "vno/supabase/SupabaseClient.hpp"
#include <iostream>

namespace vno::server {

class AuthService : public IService {
public:
    std::string name() const override { return "auth-service"; }
    void startup() override {
        auto& env = vno::config::EnvConfig::instance();
        std::cout << "[auth-service] JWT access TTL=" << env.getInt("VNO_JWT_ACCESS_TTL_MIN", 15)
                  << "min, refresh=" << env.getInt("VNO_JWT_REFRESH_TTL_DAYS", 7)
                  << "d (FR-01)\n";
        if (supabase::SupabaseClient::configured()) {
            std::string msg;
            if (supabase::SupabaseClient::healthCheck(msg))
                std::cout << "[auth-service] " << msg << " @ " << env.get("SUPABASE_URL", "") << "\n";
            else
                std::cout << "[auth-service] Supabase check failed: " << msg << "\n";
        }
    }
    void shutdown() override {}
};

} // namespace vno::server
