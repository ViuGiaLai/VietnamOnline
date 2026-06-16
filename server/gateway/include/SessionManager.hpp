#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <functional>

namespace vno::server {

class ClientSession;

/**
 * SessionManager - manages all connected player sessions
 * Doc: FR-04 Multiplayer, N.1 Session Management
 */
class SessionManager : public std::enable_shared_from_this<SessionManager> {
public:
    SessionManager();

    // Add/remove sessions
    void addSession(std::shared_ptr<ClientSession> session);
    void removeSession(const std::string& playerId);

    // Lookup
    std::shared_ptr<ClientSession> getSession(const std::string& playerId);
    std::shared_ptr<ClientSession> findBySocket(int socket);

    // Check if player is online
    bool isOnline(const std::string& playerId) const;

    // Get all online player IDs
    std::vector<std::string> getOnlinePlayerIds() const;

    // Get players in a zone
    std::vector<std::shared_ptr<ClientSession>> getZonePlayers(const std::string& zoneId);

    // Get players within radius (for interest management)
    std::vector<std::shared_ptr<ClientSession>> getNearbyPlayers(
        double x, double y, double z, 
        double radiusM, const std::string& zoneId);

    // Broadcast to all
    void broadcast(const std::string& message, const std::string& excludePlayerId = "");

    // Broadcast to zone
    void broadcastToZone(const std::string& zoneId, const std::string& message);

    // Broadcast to nearby
    void broadcastToNearby(
        double x, double y, double z,
        double radiusM, const std::string& zoneId,
        const std::string& message);

    // Count
    size_t sessionCount() const { return sessions_.size(); }

    // Cleanup timed out sessions
    void cleanupTimedOut(int timeoutSec);

    // Update player zone
    void updateZone(const std::string& playerId, const std::string& zoneId);

    // Update player position
    void updatePosition(
        const std::string& playerId,
        double x, double y, double z, float yaw,
        uint8_t state, uint8_t vehicleType);

private:
    std::unordered_map<std::string, std::shared_ptr<ClientSession>> sessions_;
    std::unordered_map<int, std::string> socketToPlayer_;
    std::unordered_map<std::string, std::string> playerZones_; // playerId -> zoneId
    mutable std::mutex mutex_;
};

} // namespace vno::server