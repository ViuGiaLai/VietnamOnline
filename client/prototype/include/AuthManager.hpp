#pragma once
#include <string>

namespace vno::client {

/// FR-01: Dang ky / dang nhap — Supabase Auth (JWT) hoac local fallback.
class AuthManager {
public:
    static bool registerAccount(const std::string& email, const std::string& password, std::string& out_player_id);
    static bool login(const std::string& email, const std::string& password, std::string& out_player_id);
    static bool devSkipEnabled();
    static bool usingSupabase();
    static const std::string& accessToken();
    static const std::string& refreshToken();
    static void clearSession();
};

} // namespace vno::client
