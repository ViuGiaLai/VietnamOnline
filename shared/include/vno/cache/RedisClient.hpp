#pragma once
#include <string>
#include <optional>

namespace vno::cache {

struct RedisConfig {
    std::string host;
    int port = 6379;
    std::string username;
    std::string password;
};

/// Redis cache layer (doc 35.1 — session, leaderboard, hot data)
class RedisClient {
public:
    static bool configured();
    static RedisConfig parseConfig();
    static bool ping(std::string& message);
    static bool set(const std::string& key, const std::string& value, int ttl_sec = 0);
    static std::optional<std::string> get(const std::string& key);
};

} // namespace vno::cache
