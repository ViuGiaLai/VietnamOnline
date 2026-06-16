#include "vno/cache/RedisClient.hpp"
#include "vno/config/EnvConfig.hpp"
#include <sstream>
#include <vector>
#include <cstring>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using socket_t = SOCKET;
constexpr socket_t kInvalidSocket = INVALID_SOCKET;
#else
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
using socket_t = int;
constexpr socket_t kInvalidSocket = -1;
#endif

namespace vno::cache {

namespace {

bool winsock_ready = false;

bool ensureNetwork() {
#ifdef _WIN32
    if (!winsock_ready) {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false;
        winsock_ready = true;
    }
#endif
    return true;
}

void closeSocket(socket_t s) {
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
}

bool sendAll(socket_t s, const std::string& data) {
    size_t sent = 0;
    while (sent < data.size()) {
#ifdef _WIN32
        int n = send(s, data.data() + sent, static_cast<int>(data.size() - sent), 0);
#else
        int n = static_cast<int>(send(s, data.data() + sent, data.size() - sent, 0));
#endif
        if (n <= 0) return false;
        sent += static_cast<size_t>(n);
    }
    return true;
}

std::string recvLine(socket_t s) {
    std::string line;
    char c;
    while (true) {
        int n = recv(s, &c, 1, 0);
        if (n <= 0) break;
        line += c;
        if (c == '\n') break;
    }
    return line;
}

std::string recvBulk(socket_t s, int len) {
    std::string data;
    data.resize(static_cast<size_t>(len));
    int got = 0;
    while (got < len) {
        int n = recv(s, &data[got], len - got, 0);
        if (n <= 0) return "";
        got += n;
    }
    recv(s, &data[0], 2, 0); // \r\n
    return data;
}

std::string readResp(socket_t s) {
    std::string line = recvLine(s);
    if (line.empty()) return "";
    char type = line[0];
    std::string payload = line.substr(1);
    if (!payload.empty() && payload.back() == '\r') payload.pop_back();

    if (type == '+' || type == '-') return payload;
    if (type == ':') return payload;
    if (type == '$') {
        int len = std::stoi(payload);
        if (len < 0) return "";
        return recvBulk(s, len);
    }
    return payload;
}

std::string urlDecode(const std::string& s) {
    std::string out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '%' && i + 2 < s.size()) {
            int v = 0;
            std::istringstream iss(s.substr(i + 1, 2));
            iss >> std::hex >> v;
            out += static_cast<char>(v);
            i += 2;
        } else {
            out += s[i];
        }
    }
    return out;
}

bool connectRedis(socket_t& out_sock, const RedisConfig& cfg) {
    if (!ensureNetwork()) return false;

    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo* res = nullptr;
    std::string port = std::to_string(cfg.port);
    if (getaddrinfo(cfg.host.c_str(), port.c_str(), &hints, &res) != 0) return false;

    socket_t s = kInvalidSocket;
    for (auto* p = res; p; p = p->ai_next) {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s == kInvalidSocket) continue;
        if (connect(s, p->ai_addr, static_cast<int>(p->ai_addrlen)) == 0) break;
        closeSocket(s);
        s = kInvalidSocket;
    }
    freeaddrinfo(res);
    if (s == kInvalidSocket) return false;

    if (!cfg.password.empty()) {
        std::string auth_cmd;
        if (!cfg.username.empty())
            auth_cmd = "AUTH " + cfg.username + " " + cfg.password + "\r\n";
        else
            auth_cmd = "AUTH " + cfg.password + "\r\n";
        if (!sendAll(s, auth_cmd)) { closeSocket(s); return false; }
        std::string auth_resp = readResp(s);
        if (auth_resp.find("OK") == std::string::npos && auth_resp.find("ok") == std::string::npos) {
            closeSocket(s);
            return false;
        }
    }

    out_sock = s;
    return true;
}

bool redisCommand(const std::vector<std::string>& args, std::string& out) {
    auto cfg = RedisClient::parseConfig();
    if (cfg.host.empty()) return false;

    socket_t s = kInvalidSocket;
    if (!connectRedis(s, cfg)) return false;

    std::ostringstream cmd;
    cmd << "*" << args.size() << "\r\n";
    for (const auto& a : args) {
        cmd << "$" << a.size() << "\r\n" << a << "\r\n";
    }
    if (!sendAll(s, cmd.str())) { closeSocket(s); return false; }

    out = readResp(s);
    closeSocket(s);
    return !out.empty();
}

} // namespace

bool RedisClient::configured() {
    auto cfg = parseConfig();
    return !cfg.host.empty();
}

RedisConfig RedisClient::parseConfig() {
    RedisConfig cfg;
    std::string url = vno::config::EnvConfig::instance().get("REDIS_URL", "");
    if (url.empty()) {
        cfg.host = "localhost";
        cfg.port = 6379;
        cfg.password = vno::config::EnvConfig::instance().get("REDIS_PASSWORD", "");
        return cfg;
    }

    std::string rest = url;
    if (rest.rfind("redis://", 0) == 0) rest = rest.substr(8);
    else if (rest.rfind("rediss://", 0) == 0) rest = rest.substr(9);

    auto at = rest.rfind('@');
    if (at != std::string::npos) {
        std::string auth = rest.substr(0, at);
        rest = rest.substr(at + 1);
        auto colon = auth.find(':');
        if (colon != std::string::npos) {
            cfg.username = urlDecode(auth.substr(0, colon));
            cfg.password = urlDecode(auth.substr(colon + 1));
        } else {
            cfg.password = urlDecode(auth);
        }
    }

    auto colon = rest.find(':');
    auto slash = rest.find('/');
    if (colon != std::string::npos) {
        cfg.host = rest.substr(0, colon);
        std::string port_str = rest.substr(colon + 1);
        if (slash != std::string::npos) port_str = port_str.substr(0, slash - colon - 1);
        cfg.port = std::stoi(port_str);
    } else {
        cfg.host = rest;
    }
  if (cfg.password.empty())
        cfg.password = vno::config::EnvConfig::instance().get("REDIS_PASSWORD", "");
    return cfg;
}

bool RedisClient::ping(std::string& message) {
    std::string resp;
    if (!redisCommand({"PING"}, resp)) {
        message = "Redis unreachable";
        return false;
    }
    message = resp;
    return resp == "PONG" || resp.find("PONG") != std::string::npos;
}

bool RedisClient::set(const std::string& key, const std::string& value, int ttl_sec) {
    std::string resp;
    if (ttl_sec > 0)
        return redisCommand({"SET", key, value, "EX", std::to_string(ttl_sec)}, resp) &&
               (resp == "OK" || resp.find("OK") != std::string::npos);
    return redisCommand({"SET", key, value}, resp) &&
           (resp == "OK" || resp.find("OK") != std::string::npos);
}

std::optional<std::string> RedisClient::get(const std::string& key) {
    std::string resp;
    if (!redisCommand({"GET", key}, resp)) return std::nullopt;
    if (resp.empty()) return std::nullopt;
    return resp;
}

} // namespace vno::cache
