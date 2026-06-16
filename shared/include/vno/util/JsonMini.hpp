#pragma once
#include <string>
#include <cctype>

namespace vno::util {

inline std::string jsonExtractString(const std::string& json, const std::string& key) {
    std::string needle = "\"" + key + "\":\"";
    auto pos = json.find(needle);
    if (pos == std::string::npos) {
        needle = "\"" + key + "\": \"";
        pos = json.find(needle);
    }
    if (pos == std::string::npos) return "";
    pos += needle.size();
    auto end = json.find('"', pos);
    if (end == std::string::npos) return "";
    return json.substr(pos, end - pos);
}

inline double jsonExtractNumber(const std::string& json, const std::string& key) {
    std::string needle = "\"" + key + "\":";
    auto pos = json.find(needle);
    if (pos == std::string::npos) return 0.0;
    pos += needle.size();
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) ++pos;
    size_t end = pos;
    while (end < json.size() &&
           (std::isdigit(static_cast<unsigned char>(json[end])) || json[end] == '.' || json[end] == '-'))
        ++end;
    if (end == pos) return 0.0;
    try { return std::stod(json.substr(pos, end - pos)); } catch (...) { return 0.0; }
}

} // namespace vno::util
