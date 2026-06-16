#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include "vno/VNO.hpp"
#include "vno/data/HoiAnZoneData.hpp"
#include "PlayerDisplay.hpp"
#include "vno/config/EnvConfig.hpp"
#include "vno/util/ChatFilter.hpp"
#include "vno/schemas/WeatherState.hpp"

using namespace vno;
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <random>
#include <chrono>
#include <algorithm>
#include <iomanip>

class GameEngine {
public:
    GameEngine();

    bool hasSave() const;
    bool loadGame();
    bool loadFromCloud();
    bool saveGame() const;

    void runOnboarding();
    void runCharacterCreator();
    void applyDailyLoginReward();
    void tickWorld(float deltaSeconds);

    void showHUD() const;
    void showMinimap() const;
    void showHelp() const;
    void showQuestLog() const;
    void showNearby() const;
    void showMapList() const;
    void showOsmInfo() const;

    bool processCommand(const std::string& input);
    void tickRetention();
    bool isRunning() const { return running_; }

    PlayerEntity& player() { return player_; }

private:
    PlayerEntity player_;
    std::vector<POI> pois_;
    std::vector<QuestEntry> quests_;
    std::vector<NPC> npcs_;
    std::vector<FoodItem> foods_;
    std::vector<VehicleConfig> vehicles_;
    std::vector<RentalStation> stations_;
    std::vector<OnlinePlayer> online_players_;
    std::vector<ChatMessage> chat_log_;

    WeatherType weather_ = WeatherType::SUNNY_CLEAR;
    WeatherState weather_state_;
    float temperature_c_ = 32.0f;
    long long last_weather_sync_ms_ = 0;
    std::vector<std::string> osm_street_names_;
    int osm_node_count_ = 0;
    int osm_enriched_ = 0;
    bool osm_from_cache_ = false;
    bool running_ = true;
    bool sprint_mode_ = false;
    void goTo(double tx, double ty);
    void onPlayerAction();
    void maybeAutoSave();
    void syncProfileToBackend();
    void showSessionSummary() const;

    std::string save_path_ = "data/saves/save.dat";
    int checkpoint_save_sec_ = 30;
    long long last_save_ms_ = 0;
    long long last_action_ms_ = 0;
    bool idle_hint_shown_ = false;
    bool broke_quest_offered_ = false;
    int session_start_vnd_ = 0;
    int met_players_session_ = 0;
    int community_checkins_ = 847;

    void initWorld();
    void spawnOnlinePlayers();
    void advanceGameClock(int minutes);
    void updateWeather();
    void updateBuffs(float deltaSeconds);
    void updateStamina(float deltaSeconds);
    void updateQuests();

    float getWalkSpeedMs() const;
    float getVehicleSpeedMs() const;
    bool canSprint() const;
    bool isOutdoor() const;
    bool isHotAfternoon() const;

    void move(double dx, double dy);
    void setState(MovementState state);

    bool interact();
    bool checkInNearestPOI();
    bool checkInPOI(const std::string& poi_id, bool from_photo = false);
    bool talkToNearestNPC();
    bool rentVehicleMenu();
    bool returnVehicle();
    bool refuelVehicle();
    bool eatAtRestaurant();
    bool buyFood(const FoodItem& food);
    bool enterPhotoMode();
    bool takePhoto();
    bool sendChat(const std::string& message);
    bool acceptQuest(const std::string& quest_id);

    POI* findPOI(const std::string& id);
    NPC* findNPC(const std::string& id);
    QuestEntry* findQuest(const std::string& id);
    const VehicleConfig* findVehicleConfig(VehicleType type) const;
    RentalStation* nearestStation(double max_dist = 15.0);

    void rewardCheckIn(POI& poi, bool first_time);
    void addVND(long long amount, const std::string& reason);
    bool spendVND(long long amount, const std::string& reason);
    void addExp(int explorer, int cultural = 0);
    void completeQuest(QuestEntry& quest);

    std::string generateId(const std::string& prefix) const;
    long long nowMs() const;
    std::string formatVND(long long v) const;

    static std::string trim(const std::string& s);
    static std::vector<std::string> split(const std::string& s, char delim);
};

#endif
