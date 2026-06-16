#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <cctype>

namespace vno::config {

/// Loads KEY=VALUE from .env (doc Ch.35 infrastructure)
class EnvConfig {
public:
    static EnvConfig& instance() {
        static EnvConfig cfg;
        return cfg;
    }

    bool load(const std::string& path = ".env") {
        std::ifstream f(path);
        if (!f) return false;
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty() || line[0] == '#') continue;
            auto pos = line.find('=');
            if (pos == std::string::npos) continue;
            std::string key = trim(line.substr(0, pos));
            std::string val = trim(line.substr(pos + 1));
            if (!key.empty()) vars_[key] = val;
        }
        return true;
    }

    std::string get(const std::string& key, const std::string& default_value = "") const {
        if (const char* env = std::getenv(key.c_str())) return std::string(env);
        auto it = vars_.find(key);
        return it != vars_.end() ? it->second : default_value;
    }

    int getInt(const std::string& key, int default_value) const {
        auto v = get(key, "");
        if (v.empty()) return default_value;
        try { return std::stoi(v); } catch (...) { return default_value; }
    }

    double getDouble(const std::string& key, double default_value) const {
        auto v = get(key, "");
        if (v.empty()) return default_value;
        try { return std::stod(v); } catch (...) { return default_value; }
    }

    bool getBool(const std::string& key, bool default_value) const {
        auto v = get(key, "");
        if (v.empty()) return default_value;
        std::string lower = v;
        for (auto& c : lower) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return lower == "1" || lower == "true" || lower == "yes" || lower == "on";
    }

    bool has(const std::string& key) const {
        if (std::getenv(key.c_str())) return true;
        return vars_.find(key) != vars_.end();
    }

private:
    EnvConfig() = default;
    std::unordered_map<std::string, std::string> vars_;

    static std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n\"'");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n\"'");
        return s.substr(start, end - start + 1);
    }
};

} // namespace vno::config
