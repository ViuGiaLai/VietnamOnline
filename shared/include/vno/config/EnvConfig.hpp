#pragma once
#include <string>
#include <unordered_map>

namespace vno::config {

/// Loads KEY=VALUE from .env (doc Ch.35 infrastructure)
class EnvConfig {
public:
    static EnvConfig& instance();

    bool load(const std::string& path = ".env");
    std::string get(const std::string& key, const std::string& default_value = "") const;
    int getInt(const std::string& key, int default_value) const;
    double getDouble(const std::string& key, double default_value) const;
    bool getBool(const std::string& key, bool default_value) const;
    bool has(const std::string& key) const;

private:
    EnvConfig() = default;
    std::unordered_map<std::string, std::string> vars_;
    static std::string trim(const std::string& s);
};

} // namespace vno::config
