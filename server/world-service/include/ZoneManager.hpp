#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>
#include "vno/config/EnvConfig.hpp"

namespace vno::server {

class ClientSession;

/**
 * ZoneManager - manages game zones/rooms
 * Doc: FR-04 Multiplayer, N.1 Zone Management
 */
class ZoneManager {
public:
    ZoneManager();
    virtual ~ZoneManager() = default;

    // Player joins zone
    bool joinZone(const std::string& playerId, const std::string& zoneId);

    // Player leaves zone
    void leaveZone(const std::string& playerId);

    // Get player's zone
    std::string getPlayerZone(const std::string& playerId) const;

    // Get players in zone
    std::vector<std::string> getZonePlayers(const std::string& zoneId) const;

    // Get player count in zone
    int getZonePlayerCount(const std::string& zoneId) const;

    // Get total player count
    int getTotalPlayerCount() const;

    // Check if zone is full
    bool isZoneFull(const std::string& zoneId) const;

    // Get zone capacity
    int getZoneCapacity(const std::string& zoneId) const;

    // Get all active zones
    std::vector<std::string> getActiveZones() const;

    // Get zone info
    struct ZoneInfo {
        std::string zoneId;
        int playerCount = 0;
        int capacity = 0;
        double centerLat = 0.0;
        double centerLng = 0.0;
    };

    ZoneInfo getZoneInfo(const std::string& zoneId) const;

    // Check if player is in zone
    bool isPlayerInZone(const std::string& playerId, const std::string& zoneId) const;

    // Get nearby players (within interest radius)
    std::vector<std::string> getNearbyPlayers(
        const std::string& playerId,
        double x, double y, double z,
        double radiusM) const;

    // Update player position (for interest management)
    void updatePlayerPosition(
        const std::string& playerId,
        double x, double y, double z);

    // Get player position
    bool getPlayerPosition(
        const std::string& playerId,
        double& outX, double& outY, double& outZ) const;

    // Get interest radius from config
    double getInterestRadius() const;

    // Get proximity chat radius from config
    double getProximityChatRadius() const;

private:
    struct Zone {
        std::string zoneId;
        int capacity = 0;
        double centerLat = 0.0;
        double centerLng = 0.0;
        std::unordered_set<std::string> players;
    };

    std::unordered_map<std::string, Zone> zones_;
    std::unordered_map<std::string, std::string> playerToZone_; // playerId -> zoneId
    std::unordered_map<std::string, std::tuple<double, double, double>> playerPositions_; // playerId -> (x, y, z)
    mutable std::mutex mutex_;

    // Initialize default zones
    void initDefaultZones();

    // Get or create zone
    Zone& getOrCreateZone(const std::string& zoneId);
};

// Inline implementation

inline ZoneManager::ZoneManager() {
    initDefaultZones();
}

inline int64_t ZoneManager::getInterestRadius() const {
    return vno::config::EnvConfig::instance().getInt("VNO_INTEREST_RADIUS_M", 200);
}

inline int64_t ZoneManager::getProximityChatRadius() const {
    return vno::config::EnvConfig::instance().getInt("VNO_PROXIMITY_CHAT_RADIUS_M", 50);
}

} // namespace vno::server