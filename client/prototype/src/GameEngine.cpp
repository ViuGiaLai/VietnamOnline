#include "GameEngine.hpp"
#include "AuthManager.hpp"
#include "vno/supabase/SupabaseClient.hpp"
#include "vno/weather/WeatherService.hpp"
#include "vno/osm/OsmClient.hpp"
#include "vno/cache/RedisClient.hpp"
#include <iostream>
#include <thread>
#include <cctype>

namespace {
std::string savePathFromEnv() {
    return vno::config::EnvConfig::instance().get("VNO_SAVE_PATH", "data/saves/save.dat");
}
}

GameEngine::GameEngine() {
    auto& env = vno::config::EnvConfig::instance();
    env.load(".env");
    save_path_ = savePathFromEnv();
    checkpoint_save_sec_ = env.getInt("VNO_CHECKPOINT_SAVE_SEC", 30);
    long long t = nowMs();
    last_action_ms_ = t;
    last_save_ms_ = t;
    initWorld();
    session_start_vnd_ = static_cast<int>(player_.vnd_balance);
}

void GameEngine::initWorld() {
    pois_ = data::createHoiAnPOIs();
    quests_ = data::createQuestRegistry();
    npcs_ = data::createHoiAnNPCs();
    foods_ = data::createFoodRegistry();
    vehicles_ = data::createVehicleRegistry();
    stations_ = data::createRentalStations();

    if (vno::osm::OsmClient::configured()) {
        std::cout << "[OSM] Dang tai du lieu OpenStreetMap (Hoi An)...\n";
        auto osm = vno::osm::OsmClient::importZone(player_.zone_id);
        if (osm.success) {
            osm_street_names_ = osm.street_names;
            osm_node_count_ = osm.node_count;
            osm_from_cache_ = osm.from_cache;
            osm_enriched_ = vno::osm::OsmClient::enrichPOIs(pois_, osm);
            std::cout << "[OSM] " << osm_node_count_ << " POI, " << osm_street_names_.size()
                      << " ten duong" << (osm_from_cache_ ? " (cache)" : " (Overpass API)") << "\n";
            std::cout << "[OSM] Credit: OpenStreetMap contributors (ODbL)\n";
        } else {
            std::cout << "[OSM] " << osm.error_message << " — dung POI noi bo.\n";
        }
    }

    last_weather_sync_ms_ = 0;
    updateWeather();
    spawnOnlinePlayers();
}

void GameEngine::spawnOnlinePlayers() {
    online_players_ = {
        {"P-101", "Minh_Khoa", 130, 190, "HOI_AN"},
        {"P-102", "Bach_Tuyet", 200, 250, "HOI_AN"},
        {"P-103", "David_Nguyen", 300, 150, "HOI_AN"},
        {"P-104", "Yuki_San", 80, 300, "HOI_AN"},
    };
}

long long GameEngine::nowMs() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

std::string GameEngine::generateId(const std::string& prefix) const {
    static std::mt19937 rng(static_cast<unsigned>(nowMs()));
    return prefix + "-" + std::to_string(rng() % 100000);
}

std::string GameEngine::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::vector<std::string> GameEngine::split(const std::string& s, char delim) {
    std::vector<std::string> parts;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        auto t = trim(item);
        if (!t.empty()) parts.push_back(t);
    }
    return parts;
}

std::string GameEngine::formatVND(long long v) const {
    std::string s = std::to_string(v);
    int insert = static_cast<int>(s.length()) - 3;
    while (insert > 0) {
        s.insert(insert, ".");
        insert -= 3;
    }
    return s + "d";
}

bool GameEngine::hasSave() const {
    std::ifstream f(save_path_);
    return f.good();
}

bool GameEngine::saveGame() const {
    std::ofstream f(save_path_);
    if (!f) return false;
    f << player_.player_id << "\n" << player_.display_name << "\n"
      << player_.gender << "\n" << player_.skin_tone << "\n"
      << player_.pos_x << " " << player_.pos_y << " " << player_.pos_z << "\n"
      << player_.zone_id << "\n"
      << player_.stamina_cur << " " << player_.stamina_max << "\n"
      << player_.vnd_balance << " " << player_.xu_vang << "\n"
      << player_.explorer_lv << " " << player_.cultural_knowledge << "\n"
      << player_.reputation_score << " " << player_.photo_score_elo << "\n"
      << static_cast<int>(player_.state) << "\n"
      << (player_.onboarding_complete ? 1 : 0) << " " << player_.login_streak << "\n"
      << player_.game_hour << " " << player_.game_minute << "\n"
      << player_.total_distance_km << "\n"
      << player_.checked_in_pois.size() << "\n";
    for (const auto& id : player_.checked_in_pois) f << id << "\n";
    f << player_.active_quest_ids.size() << "\n";
    for (const auto& id : player_.active_quest_ids) f << id << "\n";
    f << quests_.size() << "\n";
    for (const auto& q : quests_) f << q.quest_id << " " << static_cast<int>(q.status) << "\n";
    return true;
}

bool GameEngine::loadGame() {
    std::ifstream f(save_path_);
    if (!f) return false;
    int state_int = 0, onboard = 0;
    size_t checkin_count = 0, quest_active_count = 0, quest_count = 0;
    f >> player_.player_id >> player_.display_name >> player_.gender >> player_.skin_tone
      >> player_.pos_x >> player_.pos_y >> player_.pos_z >> player_.zone_id
      >> player_.stamina_cur >> player_.stamina_max
      >> player_.vnd_balance >> player_.xu_vang
      >> player_.explorer_lv >> player_.cultural_knowledge
      >> player_.reputation_score >> player_.photo_score_elo
      >> state_int >> onboard >> player_.login_streak
      >> player_.game_hour >> player_.game_minute >> player_.total_distance_km
      >> checkin_count;
    player_.state = static_cast<MovementState>(state_int);
    player_.onboarding_complete = onboard != 0;
    player_.checked_in_pois.clear();
    for (size_t i = 0; i < checkin_count; ++i) {
        std::string id; f >> id;
        player_.checked_in_pois.insert(id);
    }
    f >> quest_active_count;
    player_.active_quest_ids.clear();
    for (size_t i = 0; i < quest_active_count; ++i) {
        std::string id; f >> id;
        player_.active_quest_ids.push_back(id);
    }
    f >> quest_count;
    for (size_t i = 0; i < quest_count; ++i) {
        std::string qid; int st;
        f >> qid >> st;
        if (auto* q = findQuest(qid)) q->status = static_cast<QuestStatus>(st);
    }
    for (auto& poi : pois_) {
        poi.checked_in = player_.checked_in_pois.count(poi.poi_id) > 0;
    }
    return true;
}

bool GameEngine::loadFromCloud() {
    if (!vno::client::AuthManager::usingSupabase()) return false;
    const auto& token = vno::client::AuthManager::accessToken();
    if (token.empty() || player_.player_id.empty()) return false;

    auto result = vno::supabase::SupabaseClient::loadCharacter(token, player_.player_id, player_);
    if (!result.success) {
        std::cout << "[Cloud] Loi tai: " << result.error_message << "\n";
        return false;
    }
    if (!result.found) return false;

    for (auto& poi : pois_) {
        poi.checked_in = player_.checked_in_pois.count(poi.poi_id) > 0;
    }
    std::cout << "[Cloud] Tai profile tu Supabase: " << player_.display_name
              << " @ (" << (int)player_.pos_x << "," << (int)player_.pos_y << ")\n";
    return true;
}

void GameEngine::runCharacterCreator() {
    std::cout << "\n=== TAO NHAN VAT ===\n";
    std::cout << "Chon gioi tinh (1=Nam, 2=Nu): ";
    std::string g; std::getline(std::cin, g);
    player_.gender = (g == "2") ? "nu" : "nam";
    std::cout << "Chon tone da (1=Nhat, 2=Trung binh, 3=Dam): ";
    std::string t; std::getline(std::cin, t);
    player_.skin_tone = (t == "1") ? "light" : (t == "3") ? "dark" : "medium";
    std::cout << "Ten nhan vat (2-32 ky tu): ";
    std::getline(std::cin, player_.display_name);
    if (player_.display_name.size() < 2) player_.display_name = "Khach_Du_Lich";
    player_.player_id = generateId("VNO");
    player_.pos_x = 60;
    player_.pos_y = 220;
    player_.vnd_balance = 500000;
    player_.game_hour = 7;
    player_.game_minute = 30;
}

void GameEngine::runOnboarding() {
    std::cout << "\n[CINEMATIC] Flycam Hoi An luc binh minh... am nhac dan tranh...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
    std::cout << "[CUTSCENE] Ban buoc xuong xe khach, dat chan lan dau vao Hoi An.\n";
    std::cout << "Co Mai: \"Chao mung den Hoi An, ban oi!\"\n";
    std::cout << "[TUTORIAL] Dung WASD hoac lenh 'w/a/s/d' de di chuyen. 'h' xem huong dan.\n";
    std::cout << "[MUC TIEU] Di den Chua Cau va check-in (lenh 'e' hoac 'checkin').\n";
    player_.onboarding_step = 1;
    if (auto* q = findQuest("QUEST-ONBOARD-01")) {
        q->status = QuestStatus::AVAILABLE;
    }
}

void GameEngine::applyDailyLoginReward() {
  player_.login_streak++;
  long long reward = 100000LL * std::min(player_.login_streak, 7);
  addVND(reward, "Daily login ngay " + std::to_string(player_.login_streak));
  if (player_.login_streak == 5) player_.xu_vang += 5;
  if (player_.login_streak == 7) { player_.xu_vang += 10; addVND(500000, "Streak 7 ngay"); }
  std::cout << "Daily login! Streak: " << player_.login_streak
            << " | Thuong: " << formatVND(reward) << "\n";
}

void GameEngine::advanceGameClock(int minutes) {
    player_.game_minute += minutes;
    while (player_.game_minute >= 60) {
        player_.game_minute -= 60;
        player_.game_hour++;
        if (player_.game_hour >= 24) player_.game_hour = 0;
    }
    updateWeather();
}

void GameEngine::updateWeather() {
    auto& env = vno::config::EnvConfig::instance();
    int interval_min = env.getInt("WEATHER_SYNC_INTERVAL_MIN", 15);
    long long now = nowMs();

    if (vno::weather::WeatherService::configured() &&
        (last_weather_sync_ms_ == 0 || now - last_weather_sync_ms_ >= interval_min * 60LL * 1000LL)) {
        WeatherState ws;
        if (vno::weather::WeatherService::fetchForZone(player_.zone_id, ws)) {
            weather_ = ws.weather_type;
            temperature_c_ = ws.temperature_c;
            weather_state_ = ws;
            last_weather_sync_ms_ = now;
            std::cout << "[Weather] OpenWeatherMap: " << weatherName(weather_)
                      << " " << (int)temperature_c_ << "C (sync " << interval_min << " phut)\n";
            return;
        }
        last_weather_sync_ms_ = now;
    }

    if (player_.game_hour >= 12 && player_.game_hour < 14) {
        weather_ = WeatherType::SUNNY_HOT;
        temperature_c_ = 36.0f;
    } else if (player_.game_hour >= 18 && player_.game_hour < 21) {
        weather_ = WeatherType::CLOUDY;
        temperature_c_ = 28.0f;
    } else {
        weather_ = WeatherType::SUNNY_CLEAR;
        temperature_c_ = 32.0f;
    }
}

void GameEngine::tickWorld(float deltaSeconds) {
    updateBuffs(deltaSeconds);
    updateStamina(deltaSeconds);
    if (player_.state == MovementState::WALKING ||
        player_.state == MovementState::SPRINTING ||
        player_.state == MovementState::RIDING_VEHICLE) {
        advanceGameClock(static_cast<int>(deltaSeconds * 2));
    }
}

void GameEngine::updateBuffs(float deltaSeconds) {
    for (auto it = player_.active_buffs.begin(); it != player_.active_buffs.end();) {
        it->remaining_seconds -= static_cast<int>(deltaSeconds);
        if (it->remaining_seconds <= 0) it = player_.active_buffs.erase(it);
        else ++it;
    }
}

bool GameEngine::isOutdoor() const { return true; }
bool GameEngine::isHotAfternoon() const {
    return weather_ == WeatherType::SUNNY_HOT && player_.game_hour >= 12 && player_.game_hour < 14;
}

bool GameEngine::canSprint() const {
    return player_.stamina_cur > 0 && player_.state != MovementState::RIDING_VEHICLE;
}

float GameEngine::getWalkSpeedMs() const {
    float speed = 1.39f;
    if (isHotAfternoon() && isOutdoor()) speed = 1.11f;
    if (player_.stamina_cur <= 0) speed = 0.69f;
    for (const auto& b : player_.active_buffs) {
        if (b.buff_type == BuffType::SPEED_BOOST) speed *= (1.0f + b.value_primary);
        if (b.buff_type == BuffType::STAMINA_REGEN && b.value_secondary > 0)
            speed *= (1.0f + b.value_secondary);
    }
    if (player_.state == MovementState::SPRINTING) speed = 2.50f;
    if (weather_ == WeatherType::RAIN_LIGHT) speed *= 0.9f;
    if (weather_ == WeatherType::RAIN_HEAVY) speed *= 0.7f;
    return speed;
}

float GameEngine::getVehicleSpeedMs() const {
    if (!player_.active_vehicle) return getWalkSpeedMs();
    float kmh = player_.active_vehicle->max_speed_kmh;
    if (weather_ == WeatherType::RAIN_LIGHT) kmh *= 0.9f;
    if (weather_ == WeatherType::RAIN_HEAVY) kmh *= 0.7f;
    return kmh / 3.6f;
}

void GameEngine::updateStamina(float deltaSeconds) {
    float delta = 0.0f;
    switch (player_.state) {
        case MovementState::IDLE: delta = 2.0f; break;
        case MovementState::WALKING: delta = 0.0f; break;
        case MovementState::SPRINTING: delta = -1.0f; break;
        case MovementState::SITTING_REST: delta = 5.0f; break;
        case MovementState::EATING: delta = 0.0f; break;
        default: break;
    }
    if (isHotAfternoon() && isOutdoor() &&
        player_.state != MovementState::SITTING_REST &&
        player_.state != MovementState::EATING) {
        delta -= 0.5f;
    }
    float regen_mult = 1.0f;
    for (const auto& b : player_.active_buffs) {
        if (b.buff_type == BuffType::STAMINA_REGEN) {
            if (b.value_primary >= 1.5f) regen_mult *= b.value_primary;
            else regen_mult *= (1.0f + b.value_primary);
        }
    }
    if (player_.state == MovementState::SITTING_REST && regen_mult > 1.0f)
        delta *= regen_mult;

    player_.stamina_cur += delta * deltaSeconds;
    if (player_.stamina_cur > player_.stamina_max) player_.stamina_cur = player_.stamina_max;
    if (player_.stamina_cur < 0) {
        player_.stamina_cur = 0;
        if (player_.state == MovementState::SPRINTING) setState(MovementState::WALKING);
    }
}

void GameEngine::setState(MovementState state) {
    player_.state = state;
}

void GameEngine::move(double dx, double dy) {
    if (player_.state == MovementState::INTERACTING ||
        player_.state == MovementState::PHOTO_MODE ||
        player_.state == MovementState::EATING) {
        std::cout << "Khong the di chuyen trong trang thai hien tai.\n";
        return;
    }
    float speed = player_.active_vehicle ? getVehicleSpeedMs() : getWalkSpeedMs();
    if (!player_.active_vehicle) {
        if (sprint_mode_ && canSprint()) setState(MovementState::SPRINTING);
        else if (dx != 0 || dy != 0) setState(MovementState::WALKING);
        else { setState(MovementState::IDLE); return; }
        speed = getWalkSpeedMs();
    } else {
        setState(MovementState::RIDING_VEHICLE);
        speed = getVehicleSpeedMs();
        float dist_km = static_cast<float>(std::sqrt(dx*dx + dy*dy) * speed / 1000.0);
        player_.active_vehicle->fuel_km_remaining -= dist_km;
        player_.total_distance_km += dist_km;
        if (player_.active_vehicle->fuel_km_remaining <= 0) {
            player_.active_vehicle->fuel_km_remaining = 0;
            player_.active_vehicle.reset();
            setState(MovementState::IDLE);
            std::cout << "[XE] Het xang! Xe dung lai.\n";
        }
    }

    double len = std::sqrt(dx*dx + dy*dy);
    if (len > 0) { dx /= len; dy /= len; }
    player_.pos_x += dx * speed;
    player_.pos_y += dy * speed;
    if (player_.pos_z < 0) player_.pos_z = 0;

  if (!player_.active_vehicle) {
        float dist_km = static_cast<float>(speed / 1000.0);
        player_.total_distance_km += dist_km;
    }

    tickWorld(1.0f);
    updateQuests();
}

void GameEngine::addVND(long long amount, const std::string& reason) {
    player_.vnd_balance = std::min(500000000LL, player_.vnd_balance + amount);
    if (!reason.empty()) std::cout << "[+VND] " << formatVND(amount) << " - " << reason << "\n";
}

bool GameEngine::spendVND(long long amount, const std::string& reason) {
    if (player_.vnd_balance < amount) {
        std::cout << "Khong du tien! Can " << formatVND(amount) << "\n";
        return false;
    }
    player_.vnd_balance -= amount;
    std::cout << "[-VND] " << formatVND(amount);
    if (!reason.empty()) std::cout << " - " << reason;
    std::cout << "\n";
    return true;
}

void GameEngine::addExp(int explorer, int cultural) {
    float mult = 1.0f;
    for (const auto& b : player_.active_buffs)
        if (b.buff_type == BuffType::EXP_BONUS) mult += b.value_primary;
    int exp = static_cast<int>(explorer * mult);
    int cult = static_cast<int>(cultural * mult);
    player_.explorer_lv = static_cast<int16_t>(std::min(100, player_.explorer_lv + exp / 100));
    player_.cultural_knowledge = static_cast<int16_t>(std::min(1000, player_.cultural_knowledge + cult));
    if (exp > 0) std::cout << "[EXP] +" << exp << " Explorer";
    if (cult > 0) std::cout << ", +" << cult << " Van hoa";
    if (exp > 0 || cult > 0) std::cout << "\n";
}

POI* GameEngine::findPOI(const std::string& id) {
    for (auto& p : pois_) if (p.poi_id == id) return &p;
    return nullptr;
}

NPC* GameEngine::findNPC(const std::string& id) {
    for (auto& n : npcs_) if (n.npc_id == id) return &n;
    return nullptr;
}

QuestEntry* GameEngine::findQuest(const std::string& id) {
    for (auto& q : quests_) if (q.quest_id == id) return &q;
    return nullptr;
}

const VehicleConfig* GameEngine::findVehicleConfig(VehicleType type) const {
    for (const auto& v : vehicles_) if (v.type == type) return &v;
    return nullptr;
}

RentalStation* GameEngine::nearestStation(double max_dist) {
    RentalStation* best = nullptr;
    double best_d = max_dist;
    for (auto& s : stations_) {
        double d = vno::data::distance2D(player_.pos_x, player_.pos_y, s.x, s.y);
        if (d < best_d) { best_d = d; best = &s; }
    }
    return best;
}

void GameEngine::rewardCheckIn(POI& poi, bool first_time) {
    int exp = poi.exp_reward;
    int cultural = poi.exp_reward / 2;
    long long vnd = 50000;
    if (first_time) {
        exp = poi.first_discovery_bonus / 5;
        vnd = 200000;
        if (poi.poi_id == "POI-001" && !player_.onboarding_complete) {
            vnd = 200000;
            std::cout << "[ONBOARDING] Check-in dau tien tai Chua Cau!\n";
            player_.onboarding_step = 2;
            if (auto* q = findQuest("QUEST-ONBOARD-01")) {
                q->status = QuestStatus::ACTIVE;
                player_.active_quest_ids.push_back(q->quest_id);
            }
            std::cout << "[QUEST] Mo quest: Chup 3 buc anh Pho Co!\n";
        }
        if (poi.is_hidden) {
            if (auto* hq = findQuest("QUEST-HIDDEN-01")) hq->status = QuestStatus::AVAILABLE;
        }
    } else {
        exp = poi.exp_reward / 10;
        vnd = 10000;
    }
    addVND(vnd, "Check-in " + poi.name_vi);
    addExp(exp, cultural);
    community_checkins_++;
}

bool GameEngine::checkInPOI(const std::string& poi_id, bool from_photo) {
    POI* poi = findPOI(poi_id);
    if (!poi) return false;
    if (poi->is_hidden && player_.checked_in_pois.count(poi_id) == 0) {
        bool unlocked = false;
        for (const auto& qid : player_.active_quest_ids) {
            if (qid == "QUEST-HIDDEN-01") unlocked = true;
        }
        if (!unlocked && player_.cultural_knowledge < 50) {
            std::cout << "Dia diem bi an. Can Cultural Knowledge >= 50 hoac quest dac biet.\n";
            return false;
        }
    }
    double dist = vno::data::distance2D(player_.pos_x, player_.pos_y, poi->x, poi->y);
    if (dist > poi->check_in_radius_m) {
        std::cout << "Den gan hon de check-in! Con " << (int)dist << "m (can < "
                  << poi->check_in_radius_m << "m)\n";
        return false;
    }
    if (poi->requires_photo && !from_photo) {
        std::cout << "Dia diem nay yeu cau chup anh xac nhan. Dung lenh 'photo'.\n";
        return false;
    }
    if (poi->entry_fee_vnd > 0) {
        if (!spendVND(poi->entry_fee_vnd, "Ve tham quan " + poi->name_vi)) return false;
    }
    bool first = player_.checked_in_pois.count(poi_id) == 0;
    poi->checked_in = true;
    player_.checked_in_pois.insert(poi_id);
    std::cout << "\n*** CHECK-IN: " << poi->name_vi << " ***\n";
    std::cout << poi->description << "\n";
    rewardCheckIn(*poi, first);
    updateQuests();
    return true;
}

bool GameEngine::checkInNearestPOI() {
    POI* nearest = nullptr;
    double best = 99999;
    for (auto& p : pois_) {
        if (p.is_hidden && player_.checked_in_pois.count(p.poi_id) == 0) continue;
        double d = vno::data::distance2D(player_.pos_x, player_.pos_y, p.x, p.y);
        if (d <= p.check_in_radius_m && d < best) { best = d; nearest = &p; }
    }
    if (!nearest) {
        std::cout << "Khong co dia diem nao gan de check-in.\n";
        return false;
    }
    return checkInPOI(nearest->poi_id);
}

bool GameEngine::talkToNearestNPC() {
    NPC* nearest = nullptr;
    double best = 99999;
    for (auto& n : npcs_) {
        double d = vno::data::distance2D(player_.pos_x, player_.pos_y, n.x, n.y);
        if (d < 30 && d < best) { best = d; nearest = &n; }
    }
    if (!nearest) {
        std::cout << "Khong co NPC nao gan.\n";
        return false;
    }
    setState(MovementState::INTERACTING);
    std::cout << "\n[" << nearest->name << " - " << nearest->job << "]\n";
    if (nearest->mood == NPCMood::BUSY && !nearest->dialogue_busy.empty())
        std::cout << nearest->dialogue_busy[0] << "\n";
    else if (!nearest->dialogue_greet.empty()) {
        for (const auto& line : nearest->dialogue_greet)
            std::cout << nearest->name << ": \"" << line << "\"\n";
    }
    if (std::find(player_.met_npc_ids.begin(), player_.met_npc_ids.end(), nearest->npc_id)
        == player_.met_npc_ids.end()) {
        player_.met_npc_ids.push_back(nearest->npc_id);
    }
    if (nearest->npc_id == "NPC-BA-NAM") {
        std::cout << "[QUEST] Co the nhan quest 'Ba Nam Mat Meo' - go 'quest accept QUEST-SIDE-01'\n";
    }
    if (nearest->npc_id == "NPC-CHU-BA") {
        std::cout << "[GOI Y] Go 'rent' de thue xe tai tram gan day.\n";
    }
    if (nearest->npc_id == "NPC-CO-LAN") {
        std::cout << "[GOI Y] Go 'eat' de goi mon.\n";
    }
    setState(MovementState::IDLE);
    updateQuests();
    return true;
}

bool GameEngine::rentVehicleMenu() {
    auto* station = nearestStation(15.0);
    if (!station) {
        std::cout << "Khong co tram thue xe gan (can trong 15m).\n";
        return false;
    }
    if (player_.active_vehicle) {
        std::cout << "Ban dang thue xe! Go 'return' de tra xe.\n";
        return false;
    }
    std::cout << "\n=== " << station->name << " ===\n";
    int idx = 1;
    std::vector<VehicleType> options;
    for (auto vt : station->available) {
        if (const auto* cfg = findVehicleConfig(vt)) {
            std::cout << idx << ". " << cfg->model << " - " << formatVND(cfg->rental_cost_hour) << "/gio"
                      << " (" << (int)cfg->max_speed_kmh << " km/h)\n";
            options.push_back(vt);
            idx++;
        }
    }
    std::cout << "Chon xe (so): ";
    std::string choice; std::getline(std::cin, choice);
    int c = std::stoi(choice.empty() ? "1" : choice) - 1;
    if (c < 0 || c >= static_cast<int>(options.size())) return false;
    const auto* cfg = findVehicleConfig(options[c]);
    if (!cfg) return false;
    if (!spendVND(cfg->rental_cost_hour, "Thue " + cfg->model)) return false;

    VehicleRef ref;
    ref.type = cfg->type;
    ref.model = cfg->model;
    ref.max_speed_kmh = cfg->max_speed_kmh;
    ref.fuel_tank_km = cfg->fuel_tank_km;
    ref.fuel_km_remaining = cfg->fuel_tank_km;
    ref.rental_cost_hour = cfg->rental_cost_hour;
    ref.rental_start_time = nowMs();
    player_.active_vehicle = ref;
    setState(MovementState::RIDING_VEHICLE);
    std::cout << "[XE] Da thue " << cfg->model << "! Dung WASD de lai.\n";
    updateQuests();
    return true;
}

bool GameEngine::returnVehicle() {
    if (!player_.active_vehicle) {
        std::cout << "Ban khong thue xe nao.\n";
        return false;
    }
    std::cout << "[XE] Da tra " << player_.active_vehicle->model << ".\n";
    player_.active_vehicle.reset();
    setState(MovementState::IDLE);
    return true;
}

bool GameEngine::refuelVehicle() {
    if (!player_.active_vehicle) {
        std::cout << "Ban khong co xe.\n";
        return false;
    }
    if (!spendVND(15000, "Do xang")) return false;
    player_.active_vehicle->fuel_km_remaining = player_.active_vehicle->fuel_tank_km;
    std::cout << "[XE] Da do day binh xang!\n";
    return true;
}

bool GameEngine::buyFood(const FoodItem& food) {
    if (!food.region.empty() && food.region != "NATIONWIDE" && food.region != player_.zone_id
        && food.region != "HOI_AN" && player_.zone_id == "HOI_AN" && food.region == "HN") {
        // allow nationwide items
    }
    if (food.region == "HOI_AN" && player_.zone_id != "HOI_AN") {
        std::cout << "Mon nay chi co tai Hoi An!\n";
        return false;
    }
    if (!spendVND(food.price, "Mua " + food.name)) return false;

    BuffEntry buff = food.buff;
    buff.buff_id = generateId("BUFF");
    buff.source_food = food.name;
    buff.remaining_seconds = buff.duration_seconds;

    for (auto it = player_.active_buffs.begin(); it != player_.active_buffs.end();) {
        if (it->source_food == food.name) {
            if (buff.stack_behavior == StackBehavior::NO_STACK) {
                std::cout << "Buff nay khong stack!\n";
                return true;
            }
            it = player_.active_buffs.erase(it);
        } else ++it;
    }
    if (player_.active_buffs.size() >= 5)
        player_.active_buffs.erase(player_.active_buffs.begin());

    setState(MovementState::EATING);
    std::cout << "[AN] Dang an " << food.name << "...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    player_.active_buffs.push_back(buff);
    setState(MovementState::SITTING_REST);
    std::cout << "[BUFF] " << food.name << " - hieu luc " << buff.duration_seconds / 60 << " phut.\n";
    updateQuests();
    return true;
}

bool GameEngine::eatAtRestaurant() {
    POI* food_poi = nullptr;
    double best = 99999;
    for (auto& p : pois_) {
        if (p.category != POICategory::FOOD) continue;
        double d = vno::data::distance2D(player_.pos_x, player_.pos_y, p.x, p.y);
        if (d < 50 && d < best) { best = d; food_poi = &p; }
    }
    if (!food_poi) {
        std::cout << "Khong co nha hang gan. Di den khu pho co hoac cho dem.\n";
        return false;
    }
    std::cout << "\n=== Menu tai " << food_poi->name_vi << " ===\n";
    int i = 1;
    for (const auto& f : foods_) {
        std::cout << i << ". " << f.name << " - " << formatVND(f.price) << "\n";
        i++;
    }
    std::cout << "Chon mon (so): ";
    std::string c; std::getline(std::cin, c);
    int idx = std::stoi(c.empty() ? "1" : c) - 1;
    if (idx < 0 || idx >= static_cast<int>(foods_.size())) return false;
    return buyFood(foods_[idx]);
}

bool GameEngine::enterPhotoMode() {
    if (player_.state == MovementState::RIDING_VEHICLE) {
        returnVehicle();
    }
    setState(MovementState::PHOTO_MODE);
    std::cout << "[PHOTO MODE] HUD an. Go 'snap' de chup, 'esc' de thoat.\n";
    return true;
}

bool GameEngine::takePhoto() {
    if (player_.state != MovementState::PHOTO_MODE) {
        enterPhotoMode();
    }
    POI* nearest = nullptr;
    double best = 99999;
    for (auto& p : pois_) {
        double d = vno::data::distance2D(player_.pos_x, player_.pos_y, p.x, p.y);
        if (d < best) { best = d; nearest = &p; }
    }
    PhotoEntry photo;
    photo.photo_id = generateId("PHOTO");
    photo.timestamp_ms = nowMs();
    if (nearest && best < 100) photo.poi_id = nearest->poi_id;

    float tech = 70.0f;
    if (weather_ == WeatherType::SUNNY_CLEAR) tech += 10;
    if (weather_ == WeatherType::FOG) tech += 15;
    if (weather_ == WeatherType::SUNNY_HOT) tech -= 5;
    for (const auto& b : player_.active_buffs)
        if (b.buff_type == BuffType::PHOTO_BONUS) tech *= b.value_primary;
    photo.technical_score = std::min(100.0f, tech);
    photo.elo_score = photo.technical_score >= 90 ? 1500 : (photo.technical_score < 40 ? 800 : 1000);
    player_.photos.push_back(photo);
    player_.photo_score_elo = (player_.photo_score_elo + photo.elo_score) / 2;

    std::cout << "[CHUP] Da chup anh!";
    if (nearest && best < 100) std::cout << " Tai: " << nearest->name_vi;
    std::cout << " | Diem: " << (int)photo.technical_score << " | ELO: " << photo.elo_score << "\n";

    if (nearest && best < 100) checkInPOI(nearest->poi_id, true);

    for (auto& q : quests_) {
        if (q.status != QuestStatus::ACTIVE) continue;
        for (auto& step : q.steps) {
            if (step.done || step.type != QuestStepType::TAKE_PHOTO) continue;
            if (step.target_id == photo.poi_id || step.target_id.empty()) {
                step.current++;
                if (step.current >= step.required) step.done = true;
            }
        }
    }
    updateQuests();
    setState(MovementState::IDLE);
    return true;
}

bool GameEngine::sendChat(const std::string& message) {
    if (message.empty()) return false;
    std::string filtered = vno::util::filterChatContent(message);
    if (filtered == "[Bi loc boi he thong]") {
        std::cout << "[Chat] Tin nhan bi loc (FR-05 toxicity filter).\n";
        return false;
    }
    ChatMessage msg;
    msg.msg_id = generateId("CHAT");
    msg.channel = ChatChannel::PROXIMITY;
    msg.sender_id = player_.player_id;
    msg.sender_name = player_.display_name;
    msg.content = filtered.substr(0, 500);
    msg.timestamp_ms = nowMs();
    chat_log_.push_back(msg);
    std::cout << "[Chat 50m] " << player_.display_name << ": " << msg.content << "\n";
    for (const auto& op : online_players_) {
        double d = vno::data::distance2D(player_.pos_x, player_.pos_y, op.pos_x, op.pos_y);
        double radius = vno::config::EnvConfig::instance().getDouble("VNO_PROXIMITY_CHAT_RADIUS_M", 50.0);
        if (d < radius) {
            std::cout << "  (nearby) " << op.display_name << " nghe thay...\n";
            met_players_session_++;
        }
    }
    return true;
}

bool GameEngine::acceptQuest(const std::string& quest_id) {
    auto* q = findQuest(quest_id);
    if (!q) { std::cout << "Quest khong ton tai.\n"; return false; }
    if (q->status != QuestStatus::AVAILABLE) {
        std::cout << "Quest khong kha dung.\n"; return false;
    }
    if (player_.active_quest_ids.size() >= 3) {
        std::cout << "Toi da 3 quest active! Hoan thanh truoc.\n"; return false;
    }
    q->status = QuestStatus::ACTIVE;
    player_.active_quest_ids.push_back(quest_id);
    std::cout << "[QUEST] Nhan: " << q->title << "\n";
    return true;
}

void GameEngine::updateQuests() {
    for (auto& q : quests_) {
        if (q.status != QuestStatus::ACTIVE) continue;
        for (auto& step : q.steps) {
            if (step.done) continue;
            switch (step.type) {
                case QuestStepType::CHECKIN_POI:
                    if (step.target_id != "POI" && player_.checked_in_pois.count(step.target_id))
                        { step.current = 1; step.done = true; }
                    break;
                case QuestStepType::VISIT_COUNT:
                    step.current = static_cast<int>(player_.checked_in_pois.size());
                    if (step.current >= step.required) step.done = true;
                    break;
                case QuestStepType::TALK_NPC:
                    step.current = static_cast<int>(player_.met_npc_ids.size());
                    if (step.current >= step.required) step.done = true;
                    break;
                case QuestStepType::RENT_VEHICLE:
                    if (player_.active_vehicle || step.current > 0) {
                        step.current = 1; step.done = true;
                    }
                    break;
                case QuestStepType::TRAVEL_DISTANCE:
                    step.current = static_cast<int>(player_.total_distance_km);
                    if (step.current >= step.required) step.done = true;
                    break;
                case QuestStepType::BUY_FOOD:
                    break;
                default: break;
            }
        }
        bool all_done = true;
        for (const auto& s : q.steps) if (!s.done) { all_done = false; break; }
        if (all_done) completeQuest(q);
    }
}

void GameEngine::completeQuest(QuestEntry& quest) {
    quest.status = QuestStatus::COMPLETED;
    player_.active_quest_ids.erase(
        std::remove(player_.active_quest_ids.begin(), player_.active_quest_ids.end(), quest.quest_id),
        player_.active_quest_ids.end());
    addVND(quest.reward_vnd, "Hoan thanh: " + quest.title);
    addExp(quest.reward_exp, quest.reward_cultural);
    std::cout << "\n*** HOAN THANH QUEST: " << quest.title << " ***\n";
    if (quest.quest_id == "QUEST-MAIN-01") {
        std::cout << "[TITLE] Nha Tham Hiem Hoi An!\n";
        player_.xu_vang += 10;
    }
    if (quest.quest_id == "QUEST-ONBOARD-01") {
        player_.onboarding_complete = true;
        std::cout << "\n[TUTORIAL KET THUC] The gioi dang cho ban. Kham pha thoi!\n";
    }
}

bool GameEngine::interact() {
    if (nearestStation(10.0)) {
        std::cout << "[E] Tram thue xe gan. Go 'rent' hoac noi chuyen NPC.\n";
    }
    if (checkInNearestPOI()) return true;
    return talkToNearestNPC();
}

void GameEngine::showHUD() const {
    std::cout << "\n+--------------------------------------------------+\n";
    std::cout << "| " << player_.zone_id << " | " << player_.game_hour << ":"
              << std::setfill('0') << std::setw(2) << player_.game_minute
              << " | " << weatherName(weather_) << " " << (int)temperature_c_ << "C";
    if (!osm_street_names_.empty()) {
        std::cout << " | " << vno::osm::OsmClient::nearestStreetName(
            player_.pos_x, player_.pos_y, osm_street_names_);
    }
    std::cout << "\n";
    std::cout << "| VND: " << formatVND(player_.vnd_balance);
    std::cout << " | Stamina: " << (int)player_.stamina_cur << "/" << (int)player_.stamina_max;
    std::cout << " | Ping: 42ms\n";
    if (!player_.active_quest_ids.empty()) {
        for (const auto& qid : player_.active_quest_ids) {
            for (const auto& q : quests_) {
                if (q.quest_id == qid) {
                    int done = 0, total = static_cast<int>(q.steps.size());
                    for (const auto& s : q.steps) if (s.done) done++;
                    std::cout << "| Quest: " << q.title << " [" << done << "/" << total << "]\n";
                }
            }
        }
    }
    std::cout << "+--------------------------------------------------+\n";
}

void GameEngine::showMinimap() const {
    const int W = 41, H = 21;
    const double scale = 12.0;
    char grid[H][W];
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++) grid[y][x] = '.';

    int px = static_cast<int>((player_.pos_x + 100) / scale);
    int py = static_cast<int>((player_.pos_y) / scale);
    px = std::max(0, std::min(W - 1, px));
    py = std::max(0, std::min(H - 1, py));

    for (const auto& p : pois_) {
        if (p.is_hidden && player_.checked_in_pois.count(p.poi_id) == 0) continue;
        int x = static_cast<int>((p.x + 100) / scale);
        int y = static_cast<int>(p.y / scale);
        if (x >= 0 && x < W && y >= 0 && y < H)
            grid[y][x] = p.checked_in ? 'v' : 'P';
    }
    for (const auto& op : online_players_) {
        int x = static_cast<int>((op.pos_x + 100) / scale);
        int y = static_cast<int>(op.pos_y / scale);
        if (x >= 0 && x < W && y >= 0 && y < H && grid[y][x] == '.')
            grid[y][x] = 'o';
    }
    grid[py][px] = '@';

    std::cout << "\n=== MINIMAP HOI AN (ban do giay — FR-06) ===\n";
    std::cout << "  @ = ban | P = POI | v = da check-in | o = nguoi choi\n";
    if (!osm_street_names_.empty())
        std::cout << "  Duong: " << vno::osm::OsmClient::nearestStreetName(
            player_.pos_x, player_.pos_y, osm_street_names_) << " (OSM)\n";
    for (int y = 0; y < H; y++) {
        std::cout << "  ";
        for (int x = 0; x < W; x++) std::cout << grid[y][x];
        std::cout << "\n";
    }
}

void GameEngine::showNearby() const {
    std::cout << "\n=== GAN BAN (100m) ===\n";
    for (const auto& p : pois_) {
        if (p.is_hidden && player_.checked_in_pois.count(p.poi_id) == 0) continue;
        double d = vno::data::distance2D(player_.pos_x, player_.pos_y, p.x, p.y);
        if (d < 100) std::cout << "  [" << (int)d << "m] " << p.name_vi
            << (p.checked_in ? " (da check-in)" : "") << "\n";
    }
    for (const auto& n : npcs_) {
        double d = vno::data::distance2D(player_.pos_x, player_.pos_y, n.x, n.y);
        if (d < 100) std::cout << "  [" << (int)d << "m] NPC: " << n.name << " (" << n.job << ")\n";
    }
    for (const auto& op : online_players_) {
        double d = vno::data::distance2D(player_.pos_x, player_.pos_y, op.pos_x, op.pos_y);
        if (d < 100) std::cout << "  [" << (int)d << "m] Nguoi choi: " << op.display_name << "\n";
    }
}

void GameEngine::showMapList() const {
    std::cout << "\n=== 24 DIA DIEM HOI AN ===\n";
    int i = 1;
    for (const auto& p : pois_) {
        if (p.is_hidden && player_.checked_in_pois.count(p.poi_id) == 0) continue;
        std::cout << i++ << ". " << p.name_vi << " (" << (int)p.x << "," << (int)p.y << ")"
                  << " T" << static_cast<int>(p.tier)
                  << (p.checked_in ? " [OK]" : "") << "\n";
    }
}

void GameEngine::showOsmInfo() const {
    auto& env = vno::config::EnvConfig::instance();
    std::cout << "\n=== BAN DO & DU LIEU (FR-06) ===\n";
    std::cout << "OpenStreetMap: " << osm_node_count_ << " nodes, "
              << osm_street_names_.size() << " ten duong";
    if (osm_from_cache_) std::cout << " [cache]";
    std::cout << " | POI enriched: " << osm_enriched_ << "\n";
    std::cout << "Overpass: " << env.get("OSM_OVERPASS_URL", "n/a") << "\n";
    std::cout << "Cesium Ion: " << (env.get("CESIUM_ION_TOKEN", "").empty() ? "chua cau hinh" : "da cau hinh (UE5)") << "\n";
    std::cout << "Cache: " << env.get("MAP_TILE_CACHE_DIR", "data/cache/tiles") << "\n";
    if (!osm_street_names_.empty()) {
        std::cout << "Ten duong mau: ";
        for (size_t i = 0; i < std::min(size_t(5), osm_street_names_.size()); ++i)
            std::cout << osm_street_names_[i] << (i + 1 < std::min(size_t(5), osm_street_names_.size()) ? ", " : "");
        std::cout << "\n";
    }
    std::cout << "Credit: (c) OpenStreetMap contributors — ODbL license\n";
}

void GameEngine::showQuestLog() const {
    std::cout << "\n=== QUEST LOG ===\n";
    for (const auto& q : quests_) {
        const char* st = "???";
        switch (q.status) {
            case QuestStatus::ACTIVE: st = "ACTIVE"; break;
            case QuestStatus::AVAILABLE: st = "AVAILABLE"; break;
            case QuestStatus::COMPLETED: st = "DONE"; break;
            case QuestStatus::LOCKED: st = "LOCKED"; break;
            default: break;
        }
        std::cout << "[" << st << "] " << q.quest_id << ": " << q.title << "\n";
        std::cout << "  " << q.description << "\n";
        for (const auto& s : q.steps)
            std::cout << "  - " << s.description << " " << (s.done ? "[X]" : "[ ]") << "\n";
        std::cout << "  Thuong: " << formatVND(q.reward_vnd) << " + EXP\n";
    }
}

void GameEngine::showHelp() const {
    std::cout << "\n=== DIEU KHIEN ===\n"
              << "  w/s/a/d     - Di chuyen (bac/trung/nam/dong)\n"
              << "  shift       - Bat/tat chay (sprint)\n"
              << "  e           - Tuong tac / check-in / noi chuyen\n"
              << "  m           - Minimap\n"
              << "  map         - Danh sach dia diem\n"
              << "  osm         - Thong tin OpenStreetMap / Cesium\n"
              << "  nearby      - Xem gan ban\n"
              << "  j           - Quest log\n"
              << "  rent/return - Thue/tra xe\n"
              << "  fuel        - Do xang (15k)\n"
              << "  eat         - Goi mon\n"
              << "  photo/snap  - Che do chup anh\n"
              << "  chat <msg>  - Chat proximity 50m\n"
              << "  quest accept <id> - Nhan quest\n"
              << "  rest        - Ngoi nghi (+stamina)\n"
              << "  status      - Trang thai nhan vat\n"
              << "  save/quit   - Luu va thoat\n";
}

void GameEngine::maybeAutoSave() {
    long long now = nowMs();
    if ((now - last_save_ms_) >= static_cast<long long>(checkpoint_save_sec_) * 1000LL) {
        saveGame();
        syncProfileToBackend();
        last_save_ms_ = now;
        std::cout << "[Auto-save] Da luu tien trinh (" << checkpoint_save_sec_ << "s — FR-10)\n";
    }
}

void GameEngine::syncProfileToBackend() {
    if (!vno::client::AuthManager::usingSupabase()) {
        auto& env = vno::config::EnvConfig::instance();
        if (!env.get("DATABASE_URL", "").empty())
            std::cout << "[Sync] DATABASE_URL configured (can dung REST hoac libpq)\n";
        return;
    }
    const auto& token = vno::client::AuthManager::accessToken();
    if (token.empty()) return;

    auto result = vno::supabase::SupabaseClient::upsertCharacter(token, player_);
    if (result.success) {
        std::cout << "[Sync] Da luu profile len Supabase (FR-10)\n";
    } else {
        std::cout << "[Sync] Loi Supabase: " << result.error_message << "\n";
        std::cout << "       -> Chay schema_supabase.sql trong Supabase SQL Editor\n";
    }
}

void GameEngine::onPlayerAction() {
    last_action_ms_ = nowMs();
    idle_hint_shown_ = false;
    maybeAutoSave();
    if (!broke_quest_offered_ && player_.vnd_balance < 5000 && player_.active_quest_ids.empty()) {
        broke_quest_offered_ = true;
        std::cout << "[Retention] Het tien! Thu: quest accept QUEST-DAILY-01\n";
    }
}

void GameEngine::tickRetention() {
    long long idle_ms = nowMs() - last_action_ms_;
    if (!idle_hint_shown_ && idle_ms > 60000) {
        idle_hint_shown_ = true;
        std::cout << "\n[Co Mai]: Ban can giup gi khong? Thu 'nearby' hoac 'j' de xem quest!\n";
    }
}

void GameEngine::showSessionSummary() const {
    int earned = static_cast<int>(player_.vnd_balance) - session_start_vnd_;
    std::cout << "\n========== TOM TAT PHIEN CHOI ==========\n";
    std::cout << "Dia diem da den: " << player_.checked_in_pois.size() << "\n";
    std::cout << "VND kiem duoc: " << formatVND(std::max(0LL, static_cast<long long>(earned))) << "\n";
    std::cout << "Nguoi choi gap: " << met_players_session_ << "\n";
    std::cout << "Quest hoan thanh: ";
    int done = 0;
    for (const auto& q : quests_) if (q.status == QuestStatus::COMPLETED) done++;
    std::cout << done << "\n";
    std::cout << "========================================\n";
}

void GameEngine::goTo(double tx, double ty) {
    double dx = tx - player_.pos_x, dy = ty - player_.pos_y;
    double dist = std::sqrt(dx * dx + dy * dy);
    if (dist < 1.0) {
        player_.pos_x = tx;
        player_.pos_y = ty;
        return;
    }
    double ndx = dx / dist, ndy = dy / dist;
    double step_size = player_.active_vehicle ? 15.0 : 5.0;
    int steps = static_cast<int>(dist / step_size) + 1;
    for (int i = 0; i < steps; ++i) {
        double remain = std::sqrt(
            (tx - player_.pos_x) * (tx - player_.pos_x) +
            (ty - player_.pos_y) * (ty - player_.pos_y));
        if (remain < 1.0) break;
        move(ndx, ndy);
    }
    player_.pos_x = tx;
    player_.pos_y = ty;
    player_.state = MovementState::IDLE;
    std::cout << "Da den (" << (int)tx << ", " << (int)ty << ")\n";
}

bool GameEngine::processCommand(const std::string& input) {
    std::string cmd = trim(input);
    if (cmd.empty()) return true;
    std::string lower = cmd;
    for (auto& c : lower) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    if (lower == "quit" || lower == "exit" || lower == "q") {
        saveGame();
        syncProfileToBackend();
        showSessionSummary();
        std::cout << "Cam on da choi Vietnam Online!\n";
        running_ = false;
        return true;
    }
    if (lower == "save") {
        saveGame();
        syncProfileToBackend();
        std::cout << "Da luu game.\n";
        return true;
    }
    if (lower == "h" || lower == "help") { showHelp(); onPlayerAction(); return true; }
    if (lower == "status" || lower == "i") { vno::client::displayPlayerStatus(player_); onPlayerAction(); return true; }
    if (lower == "m") { showMinimap(); onPlayerAction(); return true; }
    if (lower == "map") { showMapList(); onPlayerAction(); return true; }
    if (lower == "osm") { showOsmInfo(); onPlayerAction(); return true; }
    if (lower == "nearby") { showNearby(); onPlayerAction(); return true; }
    if (lower == "j") { showQuestLog(); onPlayerAction(); return true; }
    if (lower == "hud") { showHUD(); onPlayerAction(); return true; }
    if (lower == "shift" || lower == "sprint") {
        sprint_mode_ = !sprint_mode_;
        std::cout << "Sprint: " << (sprint_mode_ ? "BAT" : "TAT") << "\n";
        onPlayerAction();
        return true;
    }
    if (lower == "w" || lower == "up") { move(0, 1); onPlayerAction(); return true; }
    if (lower == "s" || lower == "down") { move(0, -1); onPlayerAction(); return true; }
    if (lower == "a" || lower == "left") { move(-1, 0); onPlayerAction(); return true; }
    if (lower == "d" || lower == "right") { move(1, 0); onPlayerAction(); return true; }
    if (lower == "e" || lower == "interact" || lower == "checkin") { interact(); onPlayerAction(); return true; }
    if (lower == "rent") { rentVehicleMenu(); onPlayerAction(); return true; }
    if (lower == "return") { returnVehicle(); onPlayerAction(); return true; }
    if (lower == "fuel") { refuelVehicle(); onPlayerAction(); return true; }
    if (lower == "eat") { eatAtRestaurant(); onPlayerAction(); return true; }
    if (lower == "photo" || lower == "f8") { enterPhotoMode(); onPlayerAction(); return true; }
    if (lower == "snap" || lower == "shoot") { takePhoto(); onPlayerAction(); return true; }
    if (lower == "rest" || lower == "sit") {
        setState(MovementState::SITTING_REST);
        tickWorld(5.0f);
        setState(MovementState::IDLE);
        std::cout << "Da nghi ngoi. Stamina hoi phuc.\n";
        onPlayerAction();
        return true;
    }
    if (lower.rfind("chat ", 0) == 0) {
        if (sendChat(cmd.substr(5))) onPlayerAction();
        return true;
    }
    if (lower.rfind("quest accept ", 0) == 0) {
        if (acceptQuest(trim(cmd.substr(13)))) onPlayerAction();
        return true;
    }
    if (lower.rfind("go ", 0) == 0) {
        auto parts = split(cmd.substr(3), ' ');
        if (parts.size() >= 2) goTo(std::stod(parts[0]), std::stod(parts[1]));
        onPlayerAction();
        return true;
    }
    std::cout << "Lenh khong hop le. Go 'help' de xem huong dan.\n";
    return true;
}
