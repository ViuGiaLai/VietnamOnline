#include "AntiCheat.hpp"
#include <iostream>
#include <algorithm>

namespace vno::server {

AntiCheat::PlayerAntiCheatState& AntiCheat::getOrCreateState(
    const std::string& playerId) {
    auto it = playerStates_.find(playerId);
    if (it != playerStates_.end()) {
        return it->second;
    }
    // Create new state
    PlayerAntiCheatState state;
    state.playerId = playerId;
    state.firstInputThisSecond = std::chrono::steady_clock::now();
    return playerStates_[playerId] = state;
}

std::string AntiCheat::validatePosition(
    const std::string& playerId,
    double newX, double newY, double newZ,
    int64_t clientTimestamp,
    uint8_t state, uint8_t vehicleType) {
    
    PlayerAntiCheatState& ps = getOrCreateState(playerId);
    
    // Check speed hack
    if (ps.lastTimestamp > 0) {
        if (checkSpeedHack(
            ps.lastX, ps.lastY, ps.lastZ,
            newX, newY, newZ,
            ps.lastTimestamp, clientTimestamp,
            state, vehicleType)) {
            ps.speedViolationCount++;
            ps.totalViolations++;
            std::cout << "[anticheat] Speed hack: " << playerId 
                      << " (count: " << ps.speedViolationCount << ")\n";
        }
        
        // Check teleport
        if (checkTeleport(
            ps.lastX, ps.lastY, ps.lastZ,
            newX, newY, newZ,
            ps.lastTimestamp, clientTimestamp)) {
            ps.teleportViolationCount++;
            ps.totalViolations++;
            std::cout << "[anticheat] Teleport: " << playerId
                      << " (count: " << ps.teleportViolationCount << ")\n";
        }
    }
    
    // Update last valid position
    ps.lastX = newX;
    ps.lastY = newY;
    ps.lastZ = newZ;
    ps.lastTimestamp = clientTimestamp;
    
    // Flag if too many violations
    if (ps.totalViolations >= 5) {
        ps.flagged = true;
    }
    
    // Return error if flagged
    if (ps.flagged) {
        return "Player flagged for review";
    }
    
    return "";
}

bool AntiCheat::checkSpeedHack(
    double oldX, double oldY, double oldZ,
    double newX, double newY, double newZ,
    int64_t oldTimestamp, int64_t newTimestamp,
    uint8_t state, uint8_t vehicleType) {
    
    double speed = calculateSpeed(
        oldX, oldY, oldZ, oldTimestamp,
        newX, newY, newZ, newTimestamp);
    
    float maxSpeed = getMaxSpeedForState(state, vehicleType);
    
    return speed > maxSpeed;
}

bool AntiCheat::checkTeleport(
    double oldX, double oldY, double oldZ,
    double newX, double newY, double newZ,
    int64_t oldTimestamp, int64_t newTimestamp) {
    
    double distance = calculateDistance(
        oldX, oldY, oldZ,
        newX, newY, newZ);
    
    double dt = (newTimestamp - oldTimestamp) / 1000.0;
    if (dt <= 0) return false;
    
    double speed = distance / dt;
    
    // Max teleport speed: 500 m/s (doc B.2)
    return speed > vno::constants::MAX_TELEPORT_M_PER_SEC;
}

bool AntiCheat::checkInputFlood(const std::string& playerId, int64_t timestamp) {
    auto it = playerStates_.find(playerId);
    if (it == playerStates_.end()) return false;
    
    PlayerAntiCheatState& ps = it->second;
    
    // Check if we're in a new second
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        now - ps.firstInputThisSecond).count();
    
    if (elapsed >= 1) {
        // Reset counter
        ps.firstInputThisSecond = now;
        ps.inputsThisSecond = 0;
    }
    
    ps.inputsThisSecond++;
    
    // Max inputs per second: 120 (doc B.2)
    return ps.inputsThisSecond > vno::constants::MAX_INPUT_ACTIONS_PER_SEC;
}

void AntiCheat::recordInputAction(const std::string& playerId, int64_t timestamp) {
    PlayerAntiCheatState& ps = getOrCreateState(playerId);
    
    // Check for flood
    if (checkInputFlood(playerId, timestamp)) {
        ps.totalViolations++;
        std::cout << "[anticheat] Input flood: " << playerId << "\n";
    }
}

void AntiCheat::removePlayer(const std::string& playerId) {
    playerStates_.erase(playerId);
}

int AntiCheat::getViolationCount(const std::string& playerId) const {
    auto it = playerStates_.find(playerId);
    if (it == playerStates_.end()) return 0;
    return it->second.totalViolations;
}

void AntiCheat::resetViolations(const std::string& playerId) {
    auto it = playerStates_.find(playerId);
    if (it != playerStates_.end()) {
        it->second.speedViolationCount = 0;
        it->second.teleportViolationCount = 0;
        it->second.totalViolations = 0;
        it->second.flagged = false;
    }
}

bool AntiCheat::isFlagged(const std::string& playerId) const {
    auto it = playerStates_.find(playerId);
    if (it == playerStates_.end()) return false;
    return it->second.flagged;
}

bool AntiCheat::getLastValidPosition(
    const std::string& playerId,
    double& outX, double& outY, double& outZ,
    int64_t& outTimestamp) const {
    
    auto it = playerStates_.find(playerId);
    if (it == playerStates_.end()) return false;
    
    const PlayerAntiCheatState& ps = it->second;
    outX = ps.lastX;
    outY = ps.lastY;
    outZ = ps.lastZ;
    outTimestamp = ps.lastTimestamp;
    
    return true;
}

void AntiCheat::setLastValidPosition(
    const std::string& playerId,
    double x, double y, double z,
    int64_t timestamp) {
    
    PlayerAntiCheatState& ps = getOrCreateState(playerId);
    ps.lastX = x;
    ps.lastY = y;
    ps.lastZ = z;
    ps.lastTimestamp = timestamp;
}

} // namespace vno::server