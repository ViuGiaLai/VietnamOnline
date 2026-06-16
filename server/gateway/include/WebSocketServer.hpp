#pragma once
#include "IService.hpp"
#include "vno/config/EnvConfig.hpp"
#include "vno/cache/RedisClient.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <atomic>

namespace vno::server {

// Forward declarations
class ClientSession;
class SessionManager;

/**
 * WebSocket Server - handles TCP connections with WebSocket framing
 * Doc: FR-04 Multiplayer, N.1 Server Architecture
 */
class WebSocketServer : public IService {
public:
    using MessageHandler = std::function<void(std::shared_ptr<ClientSession>, const std::string&)>;
    using ConnectHandler = std::function<void(std::shared_ptr<ClientSession>)>;
    using DisconnectHandler = std::function<void(std::shared_ptr<ClientSession>)>;

    WebSocketServer();
    virtual ~WebSocketServer();

    std::string name() const override { return "websocket-server"; }

    void startup() override;
    void shutdown() override;

    // Register handlers
    void onMessage(MessageHandler h) { messageHandler_ = std::move(h); }
    void onConnect(ConnectHandler h) { connectHandler_ = std::move(h); }
    void onDisconnect(DisconnectHandler h) { disconnectHandler_ = std::move(h); }

    // Broadcast to all connected clients
    void broadcast(const std::string& message);

    // Send to specific client
    void sendTo(const std::string& player_id, const std::string& message);

    // Get connected count
    int connectedCount() const;

    // Check if running
    bool isRunning() const { return running_.load(); }

private:
    // Internal state
    std::atomic<bool> running_;
    int port_;
    int maxConnections_;
    int pingIntervalSec_;
    int timeoutSec_;

    // Handlers
    MessageHandler messageHandler_;
    ConnectHandler connectHandler_;
    DisconnectHandler disconnectHandler_;

    // Session management
    std::shared_ptr<SessionManager> sessionManager_;

    // Server socket (placeholder for MVP - uses select() loop)
    int serverSocket_;
    std::vector<int> clientSockets_;

    // Accept new connections (non-blocking select loop)
    void acceptLoop();

    // Handle client data
    void handleClientData(int clientSocket);

    // WebSocket frame parsing
    static std::string parseWebSocketFrame(const std::string& data);
    static std::string createWebSocketFrame(const std::string& message);

    // Ping/pong heartbeat
    void heartbeatLoop();
};

// Simple WebSocket frame opcodes
enum class WebSocketOpcode : uint8_t {
    CONTINUATION = 0x0,
    TEXT = 0x1,
    BINARY = 0x2,
    CLOSE = 0x8,
    PING = 0x9,
    PONG = 0xA
};

} // namespace vno::server