#pragma once
#include "vno/schemas/PlayerEntity.hpp"
#include <string>

namespace vno::supabase {

struct AuthResult {
    bool success = false;
    std::string user_id;
    std::string access_token;
    std::string refresh_token;
    std::string error_message;
};

struct SaveResult {
    bool success = false;
    std::string error_message;
};

struct LoadResult {
    bool success = false;
    bool found = false;
    std::string error_message;
};

/// Supabase Auth + REST (FR-01, FR-10) — uses SUPABASE_URL + SUPABASE_ANON_KEY from EnvConfig.
class SupabaseClient {
public:
    static bool configured();
    static AuthResult signUp(const std::string& email, const std::string& password);
    static AuthResult signIn(const std::string& email, const std::string& password);
    static SaveResult upsertCharacter(const std::string& access_token, const PlayerEntity& player);
    static LoadResult loadCharacter(const std::string& access_token, const std::string& user_id, PlayerEntity& out);
    static bool healthCheck(std::string& message);
};

} // namespace vno::supabase
