#include "SessionManager.hpp"
#include "ClientSession.hpp"
#include <iostream>
#include <cmath>

namespace vno::server {

SessionManager::SessionManager() = default;

void SessionManager::addSession(std::shared_ptr<ClientSession> session) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    const std::string& playerId = session->playerId();
    if (playerId.empty()) {
        // Generate temporary ID for unauthenticated session
        playerId = "temp_" + std::to_string(session->socket());
    }
    
    sessions_[playerId] = session;
    socketToPlayer_[session->socket()] = playerId;
    playerZones_[playerId] = session->zoneId();
    
    std::cout << "[session] Added: " << playerId << " (total: " << sessions_.size() << ")\n";
}

void SessionManager::removeSession(const std::string& playerId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = sessions_.find(playerId);
    if (it != sessions_.end()) {
        int socket = it->second->socket();
        socketToPlayer_.erase(socket);
        sessions_.erase(it);
        playerZones_.erase(playerId);
        
        std::cout << "[session] Removed: " << playerId << " (total: " << sessions_.size() << ")\n";
    }
}

std::shared_ptr<ClientSession> SessionManager::getSession(const std::string& playerId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = sessions_.find(playerId);
    return (it != sessions_.end()) ? it->second : nullptr;
}

std::shared_ptr<ClientSession> SessionManager::findBySocket(int socket) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = socketToPlayer_.find(socket);
    if (it != socketToPlayer_.end()) {
        auto sit = sessions_.find(it->second);
        if (sit != sessions_.end()) {
            return sit->second;
        }
    }
    return nullptr;
}

bool SessionManager::isOnline(const std::string& playerId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return sessions_.find(playerId) != sessions_.end();
}

std::vector<std::string> SessionManager::getOnlinePlayerIds() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> ids;
    ids.reserve(sessions_.size());
    for (const auto& pair : sessions_) {
        ids.push_back(pair.first);
    }
    return ids;
}

std::vector<std::shared_ptr<ClientSession>> SessionManager::getZonePlayers(
    const std::string& zoneId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::shared_ptr<ClientSession>> result;
    for (const auto& pair : sessions_) {
        if (pair.second->zoneId() == zoneId) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<std::shared_ptr<ClientSession>> SessionManager::getNearbyPlayers(
    double x, double y, double z, 
    double radiusM, const std::string& zoneId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::shared_ptr<ClientSession>> result;
    double radiusSq = radiusM * radiusM;
    
    for (const auto& pair : sessions_) {
        auto& session = pair.second;
        if (session->zoneId() != zoneId) continue;
        
        double dx = session->posX() - x;
        double dy = session->posY() - y;
        double dz = session->posZ() - z;
        
        if (dx*dx + dy*dy + dz*dz <= radiusSq) {
            result.push_back(session);
        }
    }
    return result;
}

void SessionManager::broadcast(const std::string& message, const std::string& excludePlayerId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // TODO: Send via WebSocketServer - for now just log
    std::cout << "[session] Broadcast to " << sessions_.size() 
              << " (excluding " << excludePlayerId << ")\n";
}

void SessionManager::broadcastToZone(const std::string& zoneId, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    int count = 0;
    for (const auto& pair : sessions_) {
        if (pair.second->zoneId() == zoneId) {
            // TODO: Send via WebSocketServer
            count++;
        }
    }
    std::cout << "[session] Broadcast to zone " << zoneId << ": " << count << " players\n";
}

void SessionManager::broadcastToNearby(
    double x, double y, double z,
    double radiusM, const std::string& zoneId,
    const std::string& message) {
    auto nearby = getNearbyPlayers(x, y, z, radiusM, zoneId);
    
    // TODO: Send via WebSocketServer
    std::cout << "[session] Broadcast to nearby: " << nearby.size() << " players\n";
}

void SessionManager::cleanupTimedOut(int timeoutSec) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> toRemove;
    for (const auto& pair : sessions_) {
        if (pair.second->isTimedOut(timeoutSec)) {
            toRemove.push_back(pair.first);
        }
    }
    
    for (const auto& playerId : toRemove) {
        auto it = sessions_.find(playerId);
        if (it != sessions_.end()) {
            int socket = it->second->socket();
            socketToPlayer_.erase(socket);
            sessions_.erase(it);
            playerZones_.erase(playerId);
            
            std::cout << "[session] Timed out: " << playerId << "\n";
        }
    }
}

void SessionManager::updateZone(const std::string& playerId, const std::string& zoneId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = sessions_.find(playerId);
    if (it != sessions_.end()) {
        it->second->setZoneId(zoneId);
        playerZones_[playerId] = zoneId;
    }
}

void SessionManager::updatePosition(
    const std::string& playerId,
    double x, double y, double z, float yaw,
    uint8_t state, uint8_t vehicleType) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = sessions_.find(playerId);
    if (it != sessions_.end()) {
        it->second->setPosition(x, y, z);
        it->second->setRotationYaw(yaw);
        it->second->setState(state);
        it->second->setVehicleType(vehicleType);
    }
}

} // namespace vno::server