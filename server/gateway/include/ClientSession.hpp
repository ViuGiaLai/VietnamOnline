#pragma once
#include <string>
#include <chrono>
#include <memory>
#include <cstdint>

namespace vno::server {

// Forward declaration
class SessionManager;

/**
 * ClientSession - represents a connected player
 * Doc: FR-04 Multiplayer, N.1 Session Management
 */
class ClientSession : public std::enable_shared_from_this<ClientSession> {
public:
    ClientSession(int socket, std::shared_ptr<SessionManager> manager);
    ~ClientSession();

    // Player identification
    const std::string& playerId() const { return playerId_; }
    void setPlayerId(const std::string& id) { playerId_ = id; }

    const std::string& displayName() const { return displayName_; }
    void setDisplayName(const std::string& name) { displayName_ = name; }

    // Zone
    const std::string& zoneId() const { return zoneId_; }
    void setZoneId(const std::string& z) { zoneId_ = z; }

    // Position
    double posX() const { return posX_; }
    double posY() const { return posY_; }
    double posZ() const { return posZ_; }
    void setPosition(double x, double y, double z) {
        posX_ = x; posY_ = y; posZ_ = z;
    }

    float rotationYaw() const { return rotationYaw_; }
    void setRotationYaw(float yaw) { rotationYaw_ = yaw; }

    // Movement state
    uint8_t state() const { return state_; }
    void setState(uint8_t s) { state_ = s; }

    // Vehicle
    uint8_t vehicleType() const { return vehicleType_; }
    void setVehicleType(uint8_t v) { vehicleType_ = v; }

    // Socket
    int socket() const { return socket_; }

    // Authentication
    bool authenticated() const { return authenticated_; }
    void setAuthenticated(bool a) { authenticated_ = a; }

    // Last ping time
    std::chrono::steady_clock::time_point lastPing() const { return lastPing_; }
    void updatePing() { lastPing_ = std::chrono::steady_clock::now(); }

    // Check timeout
    bool isTimedOut(int timeoutSec) const {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - lastPing_).count();
        return elapsed > timeoutSec;
    }

    // Client timestamp for latency calculation
    int64_t clientTimestamp() const { return clientTimestamp_; }
    void setClientTimestamp(int64_t ts) { clientTimestamp_ = ts; }

    // Last position for speed calculation
    double lastPosX() const { return lastPosX_; }
    double lastPosY() const { return lastPosY_; }
    double lastPosZ() const { return lastPosZ_; }
    int64_t lastPosTimestamp() const { return lastPosTimestamp_; }

    void updateLastPosition(double x, double y, double z, int64_t ts) {
        lastPosX_ = posX_;
        lastPosY_ = posY_;
        lastPosZ_ = posZ_;
        lastPosTimestamp_ = clientTimestamp_;
        posX_ = x; posY_ = y; posZ_ = z;
        clientTimestamp_ = ts;
    }

    // Calculate speed in m/s
    double calculateSpeed() const {
        if (lastPosTimestamp_ == 0) return 0.0;
        double dt = (clientTimestamp_ - lastPosTimestamp_) / 1000.0;
        if (dt <= 0) return 0.0;
        double dx = posX_ - lastPosX_;
        double dy = posY_ - lastPosY_;
        double dz = posZ_ - lastPosZ_;
        double dist = std::sqrt(dx*dx + dy*dy + dz*dz);
        return dist / dt;
    }

private:
    int socket_;
    std::shared_ptr<SessionManager> manager_;

    // Player data
    std::string playerId_;
    std::string displayName_;
    std::string zoneId_ = "HOI_AN";

    // Position
    double posX_ = 0.0;
    double posY_ = 0.0;
    double posZ_ = 0.0;
    float rotationYaw_ = 0.0f;

    // State
    uint8_t state_ = 0; // MovementState enum
    uint8_t vehicleType_ = 0; // VehicleType enum

    // Auth
    bool authenticated_ = false;

    // Timing
    std::chrono::steady_clock::time_point lastPing_;
    int64_t clientTimestamp_ = 0;
    double lastPosX_ = 0.0;
    double lastPosY_ = 0.0;
    double lastPosZ_ = 0.0;
    int64_t lastPosTimestamp_ = 0;
};

} // namespace vno::server