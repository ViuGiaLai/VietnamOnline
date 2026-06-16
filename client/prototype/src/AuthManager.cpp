#include "AuthManager.hpp"
#include "vno/supabase/SupabaseClient.hpp"
#include "vno/config/EnvConfig.hpp"
#include <fstream>
#include <unordered_map>
#include <functional>
#include <iostream>

namespace vno::client {

namespace {

struct LocalAccount {
    std::string email;
    std::string password_hash;
    std::string player_id;
};

std::string accountsPath() {
    return "data/saves/accounts.dat";
}

std::string hashPassword(const std::string& password) {
    return "hash:" + password;
}

std::unordered_map<std::string, LocalAccount>& localAccounts() {
    static std::unordered_map<std::string, LocalAccount> store;
    static bool loaded = false;
    if (!loaded) {
        loaded = true;
        std::ifstream f(accountsPath());
        std::string email, hash, pid;
        while (f >> email >> hash >> pid) {
            store[email] = {email, hash, pid};
        }
    }
    return store;
}

void persistLocal() {
    std::ofstream f(accountsPath());
    for (const auto& [email, acc] : localAccounts())
        f << acc.email << " " << acc.password_hash << " " << acc.player_id << "\n";
}

std::string& sessionAccessToken() {
    static std::string token;
    return token;
}

std::string& sessionRefreshToken() {
    static std::string token;
    return token;
}

bool registerLocal(const std::string& email, const std::string& password, std::string& out_player_id) {
    if (email.size() < 5 || password.size() < 4) return false;
    auto& store = localAccounts();
    if (store.count(email)) return false;
    out_player_id = "VNO-" + std::to_string(std::hash<std::string>{}(email) % 1000000);
    store[email] = {email, hashPassword(password), out_player_id};
    persistLocal();
    return true;
}

bool loginLocal(const std::string& email, const std::string& password, std::string& out_player_id) {
    auto& store = localAccounts();
    auto it = store.find(email);
    if (it == store.end() || it->second.password_hash != hashPassword(password)) return false;
    out_player_id = it->second.player_id;
    return true;
}

} // namespace

bool AuthManager::devSkipEnabled() {
    return config::EnvConfig::instance().getBool("VNO_DEV_SKIP_AUTH", false);
}

bool AuthManager::usingSupabase() {
    return supabase::SupabaseClient::configured();
}

const std::string& AuthManager::accessToken() {
    return sessionAccessToken();
}

const std::string& AuthManager::refreshToken() {
    return sessionRefreshToken();
}

void AuthManager::clearSession() {
    sessionAccessToken().clear();
    sessionRefreshToken().clear();
}

bool AuthManager::registerAccount(const std::string& email, const std::string& password, std::string& out_player_id) {
    if (usingSupabase()) {
        auto result = supabase::SupabaseClient::signUp(email, password);
        if (!result.success) {
            std::cout << "[Auth] " << result.error_message << "\n";
            return false;
        }
        out_player_id = result.user_id;
        sessionAccessToken() = result.access_token;
        sessionRefreshToken() = result.refresh_token;
        std::cout << "[Auth] Supabase dang ky thanh cong.\n";
        return true;
    }
    return registerLocal(email, password, out_player_id);
}

bool AuthManager::login(const std::string& email, const std::string& password, std::string& out_player_id) {
    if (usingSupabase()) {
        auto result = supabase::SupabaseClient::signIn(email, password);
        if (!result.success) {
            std::cout << "[Auth] " << result.error_message << "\n";
            return false;
        }
        out_player_id = result.user_id;
        sessionAccessToken() = result.access_token;
        sessionRefreshToken() = result.refresh_token;
        std::cout << "[Auth] Supabase dang nhap thanh cong.\n";
        return true;
    }
    return loginLocal(email, password, out_player_id);
}

} // namespace vno::client
