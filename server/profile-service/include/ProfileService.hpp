#pragma once
#include "IService.hpp"
#include "vno/config/EnvConfig.hpp"
#include "vno/supabase/SupabaseClient.hpp"
#include <iostream>

namespace vno::server {

class ProfileService : public IService {
public:
    std::string name() const override { return "profile-service"; }
    void startup() override {
        auto& env = vno::config::EnvConfig::instance();
        std::cout << "[profile-service] Checkpoint save every "
                  << env.getInt("VNO_CHECKPOINT_SAVE_SEC", 30) << "s (FR-10)\n";
        if (supabase::SupabaseClient::configured()) {
            std::cout << "[profile-service] Supabase REST -> public.characters\n";
            std::cout << "[profile-service] Chay infra/database/schema_supabase.sql neu chua co bang\n";
        } else if (env.has("DATABASE_URL")) {
            std::cout << "[profile-service] DATABASE_URL configured\n";
        }
    }
    void shutdown() override {}
};

} // namespace vno::server
