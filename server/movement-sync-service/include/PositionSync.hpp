#pragma once
#include <vector>
#include <deque>
#include <chrono>
#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>
#include "vno/constants/SyncConstants.hpp"
#include "vno/enums/PlayerEnums.hpp"

namespace vno::server {

class ClientSession;

/**
 * PositionSync - handles player position synchronization with dead reckoning
 * Doc: FR-04 Multiplayer, N.2 Position Sync, N.3 Dead Reckoning
 */
class PositionSync {
public:
    struct PositionHistory {
        double x, y, z;
        float yaw;
        uint8_t state;
        int64_t timestamp_ms;
    };

    PositionSync();
    virtual ~PositionSync() = default;

    // Update player position from client packet
    bool updatePosition(
        const std::string& playerId,
        double x, double y, double z,
        float yaw, uint8_t state, uint8_t vehicleType,
        int64_t clientTimestamp);

    // Get interpolated position for other clients
    bool getInterpolatedPosition(
        const std::string& playerId,
        double& outX, double& outY, double& outZ,
        float& outYaw);

    // Get predicted position (dead reckoning)
    bool getPredictedPosition(
        const std::string& playerId,
        double& outX, double& outY, double& outZ);

    // Get velocity for a player
    bool getVelocity(const std::string& playerId, 
        double& outVx, double& outVy, double& outVz);

    // Remove player history
    void removePlayer(const std::string& playerId);

    // Get all players in zone for world state broadcast
    std::vector<std::string> getZonePlayers(const std::string& zoneId) const;

    // Get last position
    bool getLastPosition(const std::string& playerId,
        double& outX, double& outY, double& outZ);

    // Get movement state
    uint8_t getState(const std::string& playerId) const;

    // Get vehicle type
    uint8_t getVehicleType(const std::string& playerId) const;

    // Get rotation
    float getYaw(const std::string& playerId) const;

    // Get server timestamp
    int64_t getServerTimestamp() const;

    // Set player zone
    void setZone(const std::string& playerId, const std::string& zoneId);

    // Get player zone
    std::string getZone(const std::string& playerId) const;

    // Get player count
    size_t playerCount() const { return positions_.size(); }

    // Cleanup old entries
    void cleanup(int maxAgeMs);

private:
    struct PlayerState {
        std::string playerId;
        std::string zoneId = "HOI_AN";
        
        // Current position
        double x = 0.0, y = 0.0, z = 0.0;
        float yaw = 0.0f;
        
        // Movement
        uint8_t state = 0; // MovementState
        uint8_t vehicleType = 0; // VehicleType
        
        // Velocity (calculated from history)
        double vx = 0.0, vy = 0.0, vz = 0.0;
        
        // History for dead reckoning (keep last 3 positions)
        std::deque<PositionHistory> history;
        
        // Last update
        int64_t lastUpdateMs = 0;
        int64_t lastClientTimestamp = 0;
    };

    std::unordered_map<std::string, PlayerState> positions_;

    // Calculate velocity from history
    void calculateVelocity(PlayerState& ps);

    // Get max speed for state/vehicle
    float getMaxSpeed(uint8_t state, uint8_t vehicleType) const;
};

// Inline implementation

inline PositionSync::PositionSync() = default;

inline int64_t PositionSync::getServerTimestamp() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

inline float PositionSync::getMaxSpeed(uint8_t state, uint8_t vehicleType) const {
    // Doc: A.3 Movement Speed
    // Walk: 5 km/h = 1.39 m/s
    // Sprint: 9 km/h = 2.5 m/s
    // Bicycle: 25 km/h = 6.94 m/s
    // Honda Wave: 60 km/h = 16.67 m/s
    // Yamaha Exciter: 80 km/h = 22.22 m/s
    // Honda SH: 70 km/h = 19.44 m/s
    
    switch (state) {
        case 0: // IDLE
            return 0.0f;
        case 1: // WALKING
            return 1.39f; // 5 km/h
        case 2: // SPRINTING
            return 2.5f; // 9 km/h
        case 3: // RIDING_VEHICLE
            switch (vehicleType) {
                case 0: return 6.94f; // Bicycle
                case 1: return 16.67f; // Honda Wave
                case 2: return 22.22f; // Yamaha Exciter
                case 3: return 19.44f; // Honda SH
                case 4: return 8.33f; // Xích lô
                default: return 16.67f;
            }
        case 4: // PASSENGER
            return 0.0f;
        case 5: // INTERACTING
            return 0.0f;
        case 6: // PHOTO_MODE
            return 0.0f;
        case 7: // SITTING_REST
            return 0.0f;
        case 8: // EATING
            return 0.0f;
        default:
            return 1.39f;
    }
}

} // namespace vno::server