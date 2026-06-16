#pragma once
#include <string>
#include <algorithm>
#include <vector>

namespace vno::util {

/// FR-05: Loc noi dung chat co ban (mo rong bang moderation-service sau)
inline bool containsProfanity(const std::string& text) {
    static const std::vector<std::string> blocked = {
        "dm", "dit", "fuck", "shit", "ngu", "cho chet"
    };
    std::string lower = text;
    for (auto& c : lower) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    for (const auto& w : blocked) {
        if (lower.find(w) != std::string::npos) return true;
    }
    return false;
}

inline std::string filterChatContent(const std::string& text) {
    if (containsProfanity(text)) return "[Bi loc boi he thong]";
    return text.substr(0, 500);
}

} // namespace vno::util
