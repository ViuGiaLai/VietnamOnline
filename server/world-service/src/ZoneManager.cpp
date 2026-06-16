#include "ZoneManager.hpp"
#include <iostream>
#include <cmath>

namespace vno::server {

void ZoneManager::initDefaultZones() {
    auto& env = vno::config::EnvConfig::instance();
    
    // HOI_AN zone (MVP)
    Zone hoiAn;
    hoiAn.zoneId = "HOI_AN";
    hoiAn.capacity = env.getInt("VNO_MAX_CCU_PER_ZONE", 200);
    hoiAn.centerLat = env.getDouble("VNO_ZONE_HOI_AN_LAT", 15.8794);
    hoiAn.centerLng = env.getDouble("VNO_ZONE_HOI_AN_LNG", 108.3294);
    zones_["HOI_AN"] = hoiAn;
    
    std::cout << "[zone] Initialized HOI_AN (lat=" << hoiAn.centerLat 
              << ", lng=" << hoiAn.centerLng << ")\n";
}

ZoneManager::Zone& ZoneManager::getOrCreateZone(const std::string& zoneId) {
    auto it = zones_.find(zoneId);
    if (it != zones_.end()) {
        return it->second;
    }
    
    // Create new zone with defaults
    Zone z;
    z.zoneId = zoneId;
    z.capacity = 200;
    z.centerLat = 15.8794;
    z.centerLng = 108.3294;
    
    return zones_[zoneId] = z;
}

bool ZoneManager::joinZone(const std::string& playerId, const std::string& zoneId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if zone exists
    auto zoneIt = zones_.find(zoneId);
    if (zoneIt == zones_.end()) {
        std::cerr << "[zone] Zone not found: " << zoneId << "\n";
        return false;
    }
    
    // Check if zone is full
    if ((int)zoneIt->second.players.size() >= zoneIt->second.capacity) {
        std::cerr << "[zone] Zone full: " << zoneId << "\n";
        return false;
    }
    
    // Remove from current zone if any
    auto currentIt = playerToZone_.find(playerId);
    if (currentIt != playerToZone_.end()) {
        auto& currentZone = zones_[currentIt->second];
        currentZone.players.erase(playerId);
    }
    
    // Add to new zone
    zones_[zoneId].players.insert(playerId);
    playerToZone_[playerId] = zoneId;
    
    std::cout << "[zone] Player " << playerId << " joined " << zoneId 
              << " (count: " << zones_[zoneId].players.size() << ")\n";
    
    return true;
}

void ZoneManager::leaveZone(const std::string& playerId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = playerToZone_.find(playerId);
    if (it == playerToZone_.end()) {
        return;
    }
    
    const std::string& zoneId = it->second;
    auto zoneIt = zones_.find(zoneId);
    if (zoneIt != zones_.end()) {
        zoneIt->second.players.erase(playerId);
        std::cout << "[zone] Player " << playerId << " left " << zoneId 
                  << " (count: " << zoneIt->second.players.size() << ")\n";
    }
    
    playerToZone_.erase(it);
    playerPositions_.erase(playerId);
}

std::string ZoneManager::getPlayerZone(const std::string& playerId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = playerToZone_.find(playerId);
    if (it != playerToZone_.end()) {
        return it->second;
    }
    return "";
}

std::vector<std::string> ZoneManager::getZonePlayers(
    const std::string& zoneId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = zones_.find(zoneId);
    if (it == zones_.end()) {
        return {};
    }
    
    return std::vector<std::string>(
        it->second.players.begin(), 
        it->second.players.end());
}

int ZoneManager::getZonePlayerCount(const std::string& zoneId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = zones_.find(zoneId);
    if (it == zones_.end()) {
        return 0;
    }
    return (int)it->second.players.size();
}

int ZoneManager::getTotalPlayerCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    int total = 0;
    for (const auto& pair : zones_) {
        total += (int)pair.second.players.size();
    }
    return total;
}

bool ZoneManager::isZoneFull(const std::string& zoneId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = zones_.find(zoneId);
    if (it == zones_.end()) {
        return true;
    }
    
    return (int)it->second.players.size() >= it->second.capacity;
}

int ZoneManager::getZoneCapacity(const std::string& zoneId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = zones_.find(zoneId);
    if (it == zones_.end()) {
        return 0;
    }
    return it->second.capacity;
}

std::vector<std::string> ZoneManager::getActiveZones() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> result;
    for (const auto& pair : zones_) {
        if (!pair.second.players.empty()) {
            result.push_back(pair.first);
        }
    }
    return result;
}

ZoneManager::ZoneInfo ZoneManager::getZoneInfo(
    const std::string& zoneId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ZoneInfo info;
    info.zoneId = zoneId;
    
    auto it = zones_.find(zoneId);
    if (it != zones_.end()) {
        info.playerCount = (int)it->second.players.size();
        info.capacity = it->second.capacity;
        info.centerLat = it->second.centerLat;
        info.centerLng = it->second.centerLng;
    }
    
    return info;
}

bool ZoneManager::isPlayerInZone(
    const std::string& playerId, 
    const std::string& zoneId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = playerToZone_.find(playerId);
    if (it != playerToZone_.end()) {
        return it->second == zoneId;
    }
    return false;
}

std::vector<std::string> ZoneManager::getNearbyPlayers(
    const std::string& playerId,
    double x, double y, double z,
    double radiusM) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Get player's zone
    auto it = playerToZone_.find(playerId);
    if (it == playerToZone_.end()) {
        return {};
    }
    
    const std::string& zoneId = it->second;
    auto zoneIt = zones_.find(zoneId);
    if (zoneIt == zones_.end()) {
        return {};
    }
    
    // Find nearby players
    std::vector<std::string> result;
    double radiusSq = radiusM * radiusM;
    
    for (const auto& otherId : zoneIt->second.players) {
        if (otherId == playerId) continue;
        
        auto posIt = playerPositions_.find(otherId);
        if (posIt == playerPositions_.end()) continue;
        
        double ox = std::get<0>(posIt->second);
        double oy = std::get<1>(posIt->second);
        double oz = std::get<2>(posIt->second);
        
        double dx = ox - x;
        double dy = oy - y;
        double dz = oz - z;
        
        if (dx*dx + dy*dy + dz*dz <= radiusSq) {
            result.push_back(otherId);
        }
    }
    
    return result;
}

void ZoneManager::updatePlayerPosition(
    const std::string& playerId,
    double x, double y, double z) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    playerPositions_[playerId] = std::make_tuple(x, y, z);
}

bool ZoneManager::getPlayerPosition(
    const std::string& playerId,
    double& outX, double& outY, double& outZ) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = playerPositions_.find(playerId);
    if (it == playerPositions_.end()) {
        return false;
    }
    
    outX = std::get<0>(it->second);
    outY = std::get<1>(it->second);
    outZ = std::get<2>(it->second);
    
    return true;
}

} // namespace vno::server