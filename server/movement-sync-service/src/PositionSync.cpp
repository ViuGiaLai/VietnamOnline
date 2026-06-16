#include "PositionSync.hpp"
#include "ClientSession.hpp"
#include <iostream>
#include <algorithm>

namespace vno::server {

bool PositionSync::updatePosition(
    const std::string& playerId,
    double x, double y, double z,
    float yaw, uint8_t state, uint8_t vehicleType,
    int64_t clientTimestamp) {
    
    auto it = positions_.find(playerId);
    if (it == positions_.end()) {
        // Create new player state
        PlayerState ps;
        ps.playerId = playerId;
        ps.x = x; ps.y = y; ps.z = z;
        ps.yaw = yaw;
        ps.state = state;
        ps.vehicleType = vehicleType;
        ps.lastUpdateMs = getServerTimestamp();
        ps.lastClientTimestamp = clientTimestamp;
        
        // Add to history
        PositionHistory hist{x, y, z, yaw, state, clientTimestamp};
        ps.history.push_back(hist);
        
        positions_[playerId] = ps;
        return true;
    }
    
    PlayerState& ps = it->second;
    
    // Add to history (keep last 3)
    PositionHistory hist{x, y, z, yaw, state, clientTimestamp};
    ps.history.push_back(hist);
    if (ps.history.size() > 3) {
        ps.history.pop_front();
    }
    
    // Update current position
    ps.x = x; ps.y = y; ps.z = z;
    ps.yaw = yaw;
    ps.state = state;
    ps.vehicleType = vehicleType;
    ps.lastUpdateMs = getServerTimestamp();
    ps.lastClientTimestamp = clientTimestamp;
    
    // Recalculate velocity
    calculateVelocity(ps);
    
    return true;
}

void PositionSync::calculateVelocity(PlayerState& ps) {
    if (ps.history.size() < 2) {
        ps.vx = ps.vy = ps.vz = 0.0;
        return;
    }
    
    // Calculate from last 2 positions
    auto it = ps.history.end() - 2;
    const PositionHistory& oldPos = *it;
    const PositionHistory& newPos = ps.history.back();
    
    double dt = (newPos.timestamp_ms - oldPos.timestamp_ms) / 1000.0;
    if (dt <= 0) {
        ps.vx = ps.vy = ps.vz = 0.0;
        return;
    }
    
    ps.vx = (newPos.x - oldPos.x) / dt;
    ps.vy = (newPos.y - oldPos.y) / dt;
    ps.vz = (newPos.z - oldPos.z) / dt;
}

bool PositionSync::getInterpolatedPosition(
    const std::string& playerId,
    double& outX, double& outY, double& outZ,
    float& outYaw) {
    
    auto it = positions_.find(playerId);
    if (it == positions_.end()) return false;
    
    const PlayerState& ps = it->second;
    
    // For now, just return current position
    // TODO: Implement proper interpolation based on client latency
    outX = ps.x;
    outY = ps.y;
    outZ = ps.z;
    outYaw = ps.yaw;
    
    return true;
}

bool PositionSync::getPredictedPosition(
    const std::string& playerId,
    double& outX, double& outY, double& outZ) {
    
    auto it = positions_.find(playerId);
    if (it == positions_.end()) return false;
    
    const PlayerState& ps = it->second;
    
    // Dead reckoning: predict position based on velocity
    int64_t now = getServerTimestamp();
    double dt = (now - ps.lastUpdateMs) / 1000.0;
    
    // Cap prediction window (200ms per N.3)
    dt = std::min(dt, vno::constants::DEAD_RECKONING_WINDOW_MS / 1000.0);
    
    outX = ps.x + ps.vx * dt;
    outY = ps.y + ps.vy * dt;
    outZ = ps.z + ps.vz * dt;
    
    return true;
}

bool PositionSync::getVelocity(
    const std::string& playerId,
    double& outVx, double& outVy, double& outVz) {
    
    auto it = positions_.find(playerId);
    if (it == positions_.end()) return false;
    
    const PlayerState& ps = it->second;
    outVx = ps.vx;
    outVy = ps.vy;
    outVz = ps.vz;
    
    return true;
}

void PositionSync::removePlayer(const std::string& playerId) {
    positions_.erase(playerId);
}

std::vector<std::string> PositionSync::getZonePlayers(
    const std::string& zoneId) const {
    
    std::vector<std::string> result;
    for (const auto& pair : positions_) {
        if (pair.second.zoneId == zoneId) {
            result.push_back(pair.first);
        }
    }
    return result;
}

bool PositionSync::getLastPosition(
    const std::string& playerId,
    double& outX, double& outY, double& outZ) {
    
    auto it = positions_.find(playerId);
    if (it == positions_.end()) return false;
    
    const PlayerState& ps = it->second;
    outX = ps.x;
    outY = ps.y;
    outZ = ps.z;
    
    return true;
}

uint8_t PositionSync::getState(const std::string& playerId) const {
    auto it = positions_.find(playerId);
    if (it == positions_.end()) return 0;
    return it->second.state;
}

uint8_t PositionSync::getVehicleType(const std::string& playerId) const {
    auto it = positions_.find(playerId);
    if (it == positions_.end()) return 0;
    return it->second.vehicleType;
}

float PositionSync::getYaw(const std::string& playerId) const {
    auto it = positions_.find(playerId);
    if (it == positions_.end()) return 0.0f;
    return it->second.yaw;
}

void PositionSync::setZone(const std::string& playerId, const std::string& zoneId) {
    auto it = positions_.find(playerId);
    if (it != positions_.end()) {
        it->second.zoneId = zoneId;
    }
}

std::string PositionSync::getZone(const std::string& playerId) const {
    auto it = positions_.find(playerId);
    if (it == positions_.end()) return "HOI_AN";
    return it->second.zoneId;
}

void PositionSync::cleanup(int maxAgeMs) {
    int64_t now = getServerTimestamp();
    int64_t cutoff = now - maxAgeMs;
    
    for (auto it = positions_.begin(); it != positions_.end(); ) {
        if (it->second.lastUpdateMs < cutoff) {
            it = positions_.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace vno::server