#include "vno/supabase/SupabaseClient.hpp"
#include "vno/config/EnvConfig.hpp"
#include "vno/net/HttpClient.hpp"
#include <sstream>
#include <cctype>

namespace vno::supabase {

namespace {

std::string jsonEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: out += c;
        }
    }
    return out;
}

std::string extractJsonString(const std::string& json, const std::string& key) {
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

std::string extractJsonNestedId(const std::string& json) {
    auto pos = json.find("\"user\"");
    if (pos == std::string::npos) pos = 0;
    return extractJsonString(json.substr(pos), "id");
}

double extractJsonNumber(const std::string& json, const std::string& key) {
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

int64_t extractJsonInt64(const std::string& json, const std::string& key) {
    return static_cast<int64_t>(extractJsonNumber(json, key));
}

std::string firstJsonObject(const std::string& json) {
    auto start = json.find('{');
    auto end = json.rfind('}');
    if (start == std::string::npos || end == std::string::npos || end <= start) return json;
    return json.substr(start, end - start + 1);
}

std::string supabaseUrl() {
    return vno::config::EnvConfig::instance().get("SUPABASE_URL", "");
}

std::string anonKey() {
    return vno::config::EnvConfig::instance().get("SUPABASE_ANON_KEY", "");
}

std::vector<vno::net::HttpHeader> supabaseHeaders(const std::string& bearer = "") {
    std::vector<vno::net::HttpHeader> h;
    std::string key = anonKey();
    h.push_back({"apikey", key});
    h.push_back({"Authorization", "Bearer " + (bearer.empty() ? key : bearer)});
    h.push_back({"Content-Type", "application/json"});
    return h;
}

AuthResult parseAuthResponse(const vno::net::HttpResponse& resp) {
    AuthResult r;
    if (resp.ok()) {
        r.success = true;
        r.user_id = extractJsonNestedId(resp.body);
        if (r.user_id.empty()) r.user_id = extractJsonString(resp.body, "id");
        r.access_token = extractJsonString(resp.body, "access_token");
        r.refresh_token = extractJsonString(resp.body, "refresh_token");
        if (r.access_token.empty()) {
            r.success = false;
            r.error_message = "Dang ky thanh cong — kiem tra email xac nhan (Supabase).";
        }
        return r;
    }
    r.error_message = extractJsonString(resp.body, "msg");
    if (r.error_message.empty()) r.error_message = extractJsonString(resp.body, "error_description");
    if (r.error_message.empty()) r.error_message = extractJsonString(resp.body, "message");
    if (r.error_message.empty()) r.error_message = "HTTP " + std::to_string(resp.status);
    return r;
}

} // namespace

bool SupabaseClient::configured() {
    auto url = supabaseUrl();
    auto key = anonKey();
    return !url.empty() && url.find("supabase.co") != std::string::npos && !key.empty();
}

AuthResult SupabaseClient::signUp(const std::string& email, const std::string& password) {
    AuthResult fail;
    if (!configured()) {
        fail.error_message = "Supabase chua cau hinh trong .env";
        return fail;
    }
    std::ostringstream body;
    body << "{\"email\":\"" << jsonEscape(email) << "\",\"password\":\"" << jsonEscape(password) << "\"}";
    auto resp = vno::net::httpRequest("POST", supabaseUrl() + "/auth/v1/signup",
        supabaseHeaders(), body.str());
    return parseAuthResponse(resp);
}

AuthResult SupabaseClient::signIn(const std::string& email, const std::string& password) {
    AuthResult fail;
    if (!configured()) {
        fail.error_message = "Supabase chua cau hinh trong .env";
        return fail;
    }
    std::ostringstream body;
    body << "{\"email\":\"" << jsonEscape(email) << "\",\"password\":\"" << jsonEscape(password) << "\"}";
    auto resp = vno::net::httpRequest("POST",
        supabaseUrl() + "/auth/v1/token?grant_type=password",
        supabaseHeaders(), body.str());
    return parseAuthResponse(resp);
}

SaveResult SupabaseClient::upsertCharacter(const std::string& access_token, const PlayerEntity& player) {
    SaveResult r;
    if (!configured() || access_token.empty()) {
        r.error_message = "Chua dang nhap Supabase";
        return r;
    }

    std::ostringstream body;
    body << "{"
         << "\"user_id\":\"" << jsonEscape(player.player_id) << "\","
         << "\"display_name\":\"" << jsonEscape(player.display_name) << "\","
         << "\"zone_id\":\"" << jsonEscape(player.zone_id) << "\","
         << "\"position_json\":{\"x\":" << player.pos_x << ",\"y\":" << player.pos_y
         << ",\"z\":" << player.pos_z << "},"
         << "\"appearance_json\":{\"gender\":\"" << jsonEscape(player.gender)
         << "\",\"skin_tone\":\"" << jsonEscape(player.skin_tone) << "\"},"
         << "\"stats_json\":{\"explorer_lv\":" << player.explorer_lv
         << ",\"cultural_knowledge\":" << player.cultural_knowledge
         << ",\"reputation_score\":" << player.reputation_score
         << ",\"photo_score_elo\":" << player.photo_score_elo
         << ",\"login_streak\":" << player.login_streak << "},"
         << "\"wallet_vnd\":" << player.vnd_balance
         << "}";

    auto headers = supabaseHeaders(access_token);
    headers.push_back({"Prefer", "resolution=merge-duplicates"});

    auto resp = vno::net::httpRequest("POST", supabaseUrl() + "/rest/v1/characters",
        headers, body.str());

    if (resp.ok() || resp.status == 201) {
        r.success = true;
        return r;
    }

    // PATCH fallback khi row da ton tai
    std::ostringstream patch_url;
    patch_url << supabaseUrl() << "/rest/v1/characters?user_id=eq." << player.player_id;
    auto patch_resp = vno::net::httpRequest("PATCH", patch_url.str(), headers, body.str());
    if (patch_resp.ok()) {
        r.success = true;
        return r;
    }

    r.error_message = extractJsonString(resp.body, "message");
    if (r.error_message.empty()) r.error_message = extractJsonString(patch_resp.body, "message");
    if (r.error_message.empty()) r.error_message = "HTTP " + std::to_string(resp.status);
    return r;
}

LoadResult SupabaseClient::loadCharacter(
    const std::string& access_token, const std::string& user_id, PlayerEntity& out) {
    LoadResult r;
    if (!configured() || access_token.empty() || user_id.empty()) {
        r.error_message = "Chua dang nhap Supabase";
        return r;
    }

    std::string url = supabaseUrl() + "/rest/v1/characters?user_id=eq." + user_id + "&select=*";
    auto headers = supabaseHeaders(access_token);
    auto resp = vno::net::httpRequest("GET", url, headers);

    if (resp.status == 404 || resp.body == "[]") {
        r.found = false;
        r.success = true;
        return r;
    }
    if (!resp.ok()) {
        r.error_message = extractJsonString(resp.body, "message");
        if (r.error_message.empty()) r.error_message = "HTTP " + std::to_string(resp.status);
        if (resp.status == 404 || r.error_message.find("relation") != std::string::npos)
            r.error_message += " — chay schema_supabase.sql trong Supabase SQL Editor";
        return r;
    }

    std::string row = firstJsonObject(resp.body);
    if (row.empty() || row == "{}") {
        r.found = false;
        r.success = true;
        return r;
    }

    r.found = true;
    r.success = true;
    out.player_id = user_id;
    out.display_name = extractJsonString(row, "display_name");
    out.zone_id = extractJsonString(row, "zone_id");
    if (out.zone_id.empty()) out.zone_id = "HOI_AN";
    out.vnd_balance = extractJsonInt64(row, "wallet_vnd");
    out.explorer_lv = static_cast<int16_t>(extractJsonNumber(row, "explorer_lv"));
    out.cultural_knowledge = static_cast<int16_t>(extractJsonNumber(row, "cultural_knowledge"));
    out.reputation_score = static_cast<float>(extractJsonNumber(row, "reputation_score"));

    auto pos_block = row.find("\"position_json\"");
    if (pos_block != std::string::npos) {
        std::string pos = row.substr(pos_block, 120);
        out.pos_x = extractJsonNumber(pos, "x");
        out.pos_y = extractJsonNumber(pos, "y");
        out.pos_z = extractJsonNumber(pos, "z");
    }
    auto app_block = row.find("\"appearance_json\"");
    if (app_block != std::string::npos) {
        std::string app = row.substr(app_block, 120);
        auto g = extractJsonString(app, "gender");
        auto s = extractJsonString(app, "skin_tone");
        if (!g.empty()) out.gender = g;
        if (!s.empty()) out.skin_tone = s;
    }
    auto stats_block = row.find("\"stats_json\"");
    if (stats_block != std::string::npos) {
        std::string st = row.substr(stats_block, 200);
        out.explorer_lv = static_cast<int16_t>(extractJsonNumber(st, "explorer_lv"));
        out.cultural_knowledge = static_cast<int16_t>(extractJsonNumber(st, "cultural_knowledge"));
        out.reputation_score = static_cast<float>(extractJsonNumber(st, "reputation_score"));
        out.photo_score_elo = static_cast<int32_t>(extractJsonNumber(st, "photo_score_elo"));
        out.login_streak = static_cast<int>(extractJsonNumber(st, "login_streak"));
    }
    out.onboarding_complete = true;
    return r;
}

bool SupabaseClient::healthCheck(std::string& message) {
    if (!configured()) {
        message = "Supabase URL/key missing";
        return false;
    }
    auto resp = vno::net::httpRequest("GET", supabaseUrl() + "/auth/v1/health",
        supabaseHeaders());
    if (resp.ok() || resp.status == 200) {
        message = "Supabase reachable";
        return true;
    }
    message = "HTTP " + std::to_string(resp.status);
    return false;
}

} // namespace vno::supabase
