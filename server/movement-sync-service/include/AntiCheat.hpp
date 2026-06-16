#pragma once
#include <string>
#include <unordered_map>
#include <chrono>
#include <cmath>
#include "vno/constants/SyncConstants.hpp"

namespace vno::server {

class ClientSession;

/**
 * AntiCheat - validates player movement to prevent cheating
 * Doc: FR-04 Anti-Cheat, B.2 / IV Speed & Teleport Detection
 */
class AntiCheat {
public:
    AntiCheat();
    virtual ~AntiCheat() = default;

    // Validate position update from client
    // Returns: empty string if valid, error message if invalid
    std::string validatePosition(
        const std::string& playerId,
        double newX, double newY, double newZ,
        int64_t clientTimestamp,
        uint8_t state, uint8_t vehicleType);

    // Check speed hack
    bool checkSpeedHack(
        double oldX, double oldY, double oldZ,
        double newX, double newY, double newZ,
        int64_t oldTimestamp, int64_t newTimestamp,
        uint8_t state, uint8_t vehicleType);

    // Check teleport
    bool checkTeleport(
        double oldX, double oldY, double oldZ,
        double newX, double newY, double newZ,
        int64_t oldTimestamp, int64_t newTimestamp);

    // Check input flood
    bool checkInputFlood(const std::string& playerId, int64_t timestamp);

    // Record input action
    void recordInputAction(const std::string& playerId, int64_t timestamp);

    // Remove player from tracking
    void removePlayer(const std::string& playerId);

    // Get violation count
    int getViolationCount(const std::string& playerId) const;

    // Reset violations
    void resetViolations(const std::string& playerId);

    // Check if player is flagged
    bool isFlagged(const std::string& playerId) const;

    // Get last valid position
    bool getLastValidPosition(
        const std::string& playerId,
        double& outX, double& outY, double& outZ,
        int64_t& outTimestamp) const;

    // Set last valid position
    void setLastValidPosition(
        const std::string& playerId,
        double x, double y, double z,
        int64_t timestamp);

    // Get max speed for state/vehicle
    static float getMaxSpeedForState(uint8_t state, uint8_t vehicleType);

    // Calculate distance
    static double calculateDistance(
        double x1, double y1, double z1,
        double x2, double y2, double z2);

    // Calculate speed in m/s
    static double calculateSpeed(
        double x1, double y1, double z1, int64_t t1,
        double x2, double y2, double z2, int64_t t2);

private:
    struct PlayerAntiCheatState {
        std::string playerId;
        
        // Last valid position
        double lastX = 0.0, lastY = 0.0, lastZ = 0.0;
        int64_t lastTimestamp = 0;
        
        // Input flood tracking
        std::chrono::steady_clock::time_point firstInputThisSecond;
        int inputsThisSecond = 0;
        
        // Violation tracking
        int speedViolationCount = 0;
        int teleportViolationCount = 0;
        int totalViolations = 0;
        
        // Flagged for review
        bool flagged = false;
    };

    std::unordered_map<std::string, PlayerAntiCheatState> playerStates_;

    // Get or create player state
    PlayerAntiCheatState& getOrCreateState(const std::string& playerId);
};

// Inline implementation

inline AntiCheat::AntiCheat() = default;

inline float AntiCheat::getMaxSpeedForState(uint8_t state, uint8_t vehicleType) {
    // Doc: A.3 Movement Speed + Anti-cheat multiplier
    constexpr float MULTIPLIER = vno::constants::MAX_SPEED_HACK_MULTIPLIER; // 1.5x
    
    float baseSpeed;
    switch (state) {
        case 0: // IDLE
            baseSpeed = 0.0f; break;
        case 1: // WALKING
            baseSpeed = 1.39f; break; // 5 km/h
        case 2: // SPRINTING
            baseSpeed = 2.5f; break; // 9 km/h
        case 3: // RIDING_VEHICLE
            switch (vehicleType) {
                case 0: baseSpeed = 6.94f; break; // Bicycle
                case 1: baseSpeed = 16.67f; break; // Wave
                case 2: baseSpeed = 22.22f; break; // Exciter
                case 3: baseSpeed = 19.44f; break; // SH
                case 4: baseSpeed = 8.33f; break; // Xích lô
                default: baseSpeed = 16.67f;
            }
            break;
        default:
            baseSpeed = 1.39f;
    }
    
    return baseSpeed * MULTIPLIER;
}

inline double AntiCheat::calculateDistance(
    double x1, double y1, double z1,
    double x2, double y2, double z2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    double dz = z2 - z1;
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

inline double AntiCheat::calculateSpeed(
    double x1, double y1, double z1, int64_t t1,
    double x2, double y2, double z2, int64_t t2) {
    if (t2 <= t1) return 0.0;
    double dt = (t2 - t1) / 1000.0; // ms to seconds
    if (dt <= 0) return 0.0;
    double dist = calculateDistance(x1, y1, z1, x2, y2, z2);
    return dist / dt;
}

} // namespace vno::server