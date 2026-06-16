#include "WebSocketServer.hpp"
#include "ClientSession.hpp"
#include "SessionManager.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

namespace vno::server {

WebSocketServer::WebSocketServer()
    : running_(false)
    , port_(8081)
    , maxConnections_(50)
    , pingIntervalSec_(5)
    , timeoutSec_(30)
    , serverSocket_(-1)
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

WebSocketServer::~WebSocketServer() {
    shutdown();
#ifdef _WIN32
    WSACleanup();
#endif
}

void WebSocketServer::startup() {
    auto& env = vno::config::EnvConfig::instance();
    port_ = env.getInt("VNO_WEBSOCKET_PORT", 8081);
    maxConnections_ = env.getInt("VNO_WEBSOCKET_MAX_CONNECTIONS", 50);
    pingIntervalSec_ = env.getInt("VNO_WEBSOCKET_PING_INTERVAL_SEC", 5);
    timeoutSec_ = env.getInt("VNO_WEBSOCKET_TIMEOUT_SEC", 30);

    std::cout << "[websocket] Starting on port " << port_ << "...\n";

    // Create server socket
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ < 0) {
        std::cerr << "[websocket] Failed to create socket\n";
        return;
    }

    // Set socket options
    int opt = 1;
    setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(static_cast<uint16_t>(port_));

    if (bind(serverSocket_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "[websocket] Failed to bind port " << port_ << "\n";
        return;
    }

    // Listen
    if (listen(serverSocket_, maxConnections_) < 0) {
        std::cerr << "[websocket] Failed to listen\n";
        return;
    }

    // Set non-blocking
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(serverSocket_, FIONBIO, &mode);
#else
    fcntl(serverSocket_, F_SETFL, O_NONBLOCK);
#endif

    running_.store(true);
    std::cout << "[websocket] Listening on port " << port_ << " (max " << maxConnections_ << " connections)\n";

    // Initialize session manager
    sessionManager_ = std::make_shared<SessionManager>();

    // Start accept loop in background thread
    std::thread(&WebSocketServer::acceptLoop, this).detach();
    std::thread(&WebSocketServer::heartbeatLoop, this).detach();
}

void WebSocketServer::shutdown() {
    if (!running_.load()) return;
    running_.store(false);

    std::cout << "[websocket] Shutting down...\n";

    // Close all client sockets
    for (int sock : clientSockets_) {
        if (sock >= 0) {
#ifdef _WIN32
            closesocket(sock);
#else
            close(sock);
#endif
        }
    }
    clientSockets_.clear();

    // Close server socket
    if (serverSocket_ >= 0) {
#ifdef _WIN32
        closesocket(serverSocket_);
#else
        close(serverSocket_);
#endif
        serverSocket_ = -1;
    }

    std::cout << "[websocket] Stopped\n";
}

void WebSocketServer::acceptLoop() {
    while (running_.load()) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        int clientSocket = accept(serverSocket_, (struct sockaddr*)&clientAddr, &clientLen);
        
        if (clientSocket < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }
            break;
        }

        // Check max connections
        if ((int)clientSockets_.size() >= maxConnections_) {
            std::cout << "[websocket] Max connections reached, rejecting\n";
#ifdef _WIN32
            closesocket(clientSocket);
#else
            close(clientSocket);
#endif
            continue;
        }

        // Set non-blocking
#ifdef _WIN32
        u_long mode = 1;
        ioctlsocket(clientSocket, FIONBIO, &mode);
#else
        fcntl(clientSocket, F_SETFL, O_NONBLOCK);
#endif

        clientSockets_.push_back(clientSocket);

        // Create session
        auto session = std::make_shared<ClientSession>(clientSocket, sessionManager_);
        sessionManager_->addSession(session);

        std::cout << "[websocket] New connection from " 
                  << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port)
                  << " (total: " << clientSockets_.size() << ")\n";

        // Notify connect handler
        if (connectHandler_) {
            connectHandler_(session);
        }
    }
}

void WebSocketServer::handleClientData(int clientSocket) {
    char buffer[4096];
    ssize_t bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes <= 0) {
        // Client disconnected
        if (bytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            return;
        }
        
        // Find and remove session
        auto session = sessionManager_->findBySocket(clientSocket);
        if (session) {
            std::cout << "[websocket] Client disconnected: " << session->playerId() << "\n";
            if (disconnectHandler_) {
                disconnectHandler_(session);
            }
            sessionManager_->removeSession(session->playerId());
        }
        
        // Remove from socket list
        for (auto it = clientSockets_.begin(); it != clientSockets_.end(); ++it) {
            if (*it == clientSocket) {
                clientSockets_.erase(it);
                break;
            }
        }
        
#ifdef _WIN32
        closesocket(clientSocket);
#else
        close(clientSocket);
#endif
        return;
    }

    buffer[bytes] = '\0';

    // Find session
    auto session = sessionManager_->findBySocket(clientSocket);
    if (!session) return;

    // Parse WebSocket frame
    std::string message = parseWebSocketFrame(std::string(buffer, bytes));
    
    if (!message.empty() && messageHandler_) {
        messageHandler_(session, message);
    }
}

void WebSocketServer::broadcast(const std::string& message) {
    std::string frame = createWebSocketFrame(message);
    
    for (int sock : clientSockets_) {
        send(sock, frame.c_str(), frame.size(), 0);
    }
}

void WebSocketServer::sendTo(const std::string& player_id, const std::string& message) {
    auto session = sessionManager_->getSession(player_id);
    if (!session) return;
    
    std::string frame = createWebSocketFrame(message);
    send(session->socket(), frame.c_str(), frame.size(), 0);
}

int WebSocketServer::connectedCount() const {
    return (int)clientSockets_.size();
}

// Simple WebSocket frame parsing (TEXT frames only)
std::string WebSocketServer::parseWebSocketFrame(const std::string& data) {
    if (data.size() < 2) return "";
    
    uint8_t firstByte = static_cast<uint8_t>(data[0]);
    uint8_t opcode = firstByte & 0x0F;
    
    // Only handle TEXT frames
    if (opcode != 0x01 && opcode != 0x00) {
        if (opcode == 0x08) {
            // Close frame
            return "";
        }
        return "";
    }
    
    uint8_t secondByte = static_cast<uint8_t>(data[1]);
    bool masked = (secondByte & 0x80) != 0;
    
    size_t payloadLength = secondByte & 0x7F;
    size_t offset = 2;
    
    if (payloadLength == 126) {
        if (data.size() < 4) return "";
        payloadLength = (static_cast<uint8_t>(data[2]) << 8) | static_cast<uint8_t>(data[3]);
        offset = 4;
    } else if (payloadLength == 127) {
        if (data.size() < 10) return "";
        offset = 10;
        payloadLength = 0; // Not handling large frames for MVP
    }
    
    if (data.size() < offset + payloadLength) return "";
    
    std::string payload = data.substr(offset, payloadLength);
    
    // Unmask if needed
    if (masked && payload.size() >= 4) {
        uint8_t mask[4];
        memcpy(mask, payload.data(), 4);
        
        for (size_t i = 0; i < payload.size() - 4; ++i) {
            payload[i + 4] ^= mask[i % 4];
        }
        payload = payload.substr(4);
    }
    
    return payload;
}

std::string WebSocketServer::createWebSocketFrame(const std::string& message) {
    std::string frame;
    frame.reserve(message.size() + 10);
    
    // FIN + TEXT opcode
    frame.push_back(static_cast<char>(0x80 | 0x01));
    
    // Payload length
    if (message.size() < 126) {
        frame.push_back(static_cast<char>(message.size()));
    } else if (message.size() < 65536) {
        frame.push_back(static_cast<char>(126));
        frame.push_back(static_cast<char>((message.size() >> 8) & 0xFF));
        frame.push_back(static_cast<char>(message.size() & 0xFF));
    } else {
        frame.push_back(static_cast<char>(127));
        // 8 bytes for extended payload length
        for (int i = 7; i >= 0; --i) {
            frame.push_back(static_cast<char>((message.size() >> (i * 8)) & 0xFF));
        }
    }
    
    frame += message;
    return frame;
}

void WebSocketServer::heartbeatLoop() {
    std::string pingFrame = createWebSocketFrame("{\"type\":\"ping\"}");
    
    while (running_.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(pingIntervalSec_));
        
        if (!running_.load()) break;
        
        // Send ping to all clients
        for (int sock : clientSockets_) {
            send(sock, pingFrame.c_str(), pingFrame.size(), 0);
        }
    }
}

} // namespace vno::server