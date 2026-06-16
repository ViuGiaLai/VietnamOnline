#pragma once

#include "vno/VNO.hpp"
#include <vector>
#include <cmath>

namespace vno::data {

using vno::POI;
using vno::FoodItem;
using vno::VehicleConfig;
using vno::NPC;
using vno::RentalStation;
using vno::QuestEntry;
using vno::QuestStep;
using vno::QuestType;
using vno::QuestStatus;
using vno::QuestStepType;
using vno::BuffEntry;
using vno::BuffType;
using vno::StackBehavior;
using vno::POITier;
using vno::POICategory;
using vno::VehicleType;
using vno::NPCActivity;
using vno::NPCMood;

inline std::vector<POI> createHoiAnPOIs() {
    return {
        {"POI-001", "Chua Cau", "Japanese Covered Bridge", 100, 100, 15.8771, 108.3293,
         POITier::T1, POICategory::HERITAGE, 100, false, 0, 800, 4000, false,
         "Cau Nhat Ban bieu tuong Hoi An, xay dau the ky XVII."},
        {"POI-002", "Nha Co Tan Ky", "Tan Ky Old House", 180, 120, 15.8775, 108.3288,
         POITier::T1, POICategory::HISTORY, 100, false, 120000, 600, 3000, false,
         "Nha co 200 nam tuoi, kien truc giao thoa Viet - Hoa - Nhat."},
        {"POI-003", "Hoi Quan Phuc Kien", "Fujian Assembly Hall", 220, 90, 15.8778, 108.3285,
         POITier::T1, POICategory::SPIRITUAL, 100, false, 80000, 500, 2500, false,
         "Hoi quan cua nguoi Hoa, tho Thien Hau Thanh Mau."},
        {"POI-004", "Hoi Quan Quang Dong", "Cantonese Assembly Hall", 250, 140, 15.8780, 108.3290,
         POITier::T2, POICategory::SPIRITUAL, 75, false, 50000, 300, 1500, false,
         "Kien truc Trung Hoa voi tuong rong va hoa van tinh xao."},
        {"POI-005", "Chua Quan Cong", "Quan Cong Temple", 140, 160, 15.8773, 108.3295,
         POITier::T2, POICategory::SPIRITUAL, 75, false, 0, 250, 1250, false,
         "Den tho Quan Cong va hai thuoc ha."},
        {"POI-006", "Bao Tang Hoi An", "Hoi An Museum", 300, 200, 15.8785, 108.3300,
         POITier::T2, POICategory::HISTORY, 75, false, 120000, 400, 2000, false,
         "Trung bay hien vat lich su va van hoa Hoi An."},
        {"POI-007", "Cho Hoi An", "Hoi An Central Market", 50, 250, 15.8765, 108.3305,
         POITier::T2, POICategory::FOOD, 75, false, 0, 200, 1000, false,
         "Cho truyen thong voi dac san dia phuong."},
        {"POI-008", "Pho Co Hoi An", "Ancient Town", 120, 180, 15.8770, 108.3290,
         POITier::T1, POICategory::HERITAGE, 100, false, 120000, 700, 3500, false,
         "Khu pho co UNESCO voi hang tram ngo nha co."},
        {"POI-009", "Den Long Hoi An", "Lantern Street", 160, 220, 15.8768, 108.3298,
         POITier::T2, POICategory::ENTERTAINMENT, 75, false, 0, 300, 1500, false,
         "Pho den long lung linh ve dem."},
        {"POI-010", "Chua Phuoc Lam", "Phuoc Lam Pagoda", 400, 150, 15.8790, 108.3280,
         POITier::T3, POICategory::SPIRITUAL, 50, false, 0, 100, 500, false,
         "Ngoi chua co kien truc moc ban."},
        {"POI-011", "Lang Gom Thanh Ha", "Thanh Ha Pottery Village", 550, 300, 15.8800, 108.3320,
         POITier::T2, POICategory::HISTORY, 75, false, 50000, 350, 1750, false,
         "Lang nghe gom truyen thong hon 500 nam."},
        {"POI-012", "Lang Rau Tra Que", "Tra Que Vegetable Village", 600, 100, 15.8810, 108.3270,
         POITier::T2, POICategory::NATURE, 75, false, 0, 300, 1500, false,
         "Lang rau huu co noi tieng voi Cao Lau Hoi An."},
        {"POI-013", "Bai Bien An Bang", "An Bang Beach", 700, 400, 15.8850, 108.3450,
         POITier::T2, POICategory::NATURE, 75, false, 0, 400, 2000, false,
         "Bai bien xanh voi ca phe ven bo."},
        {"POI-014", "Bai Bien Cua Dai", "Cua Dai Beach", 750, 450, 15.8880, 108.3500,
         POITier::T3, POICategory::NATURE, 50, false, 0, 150, 750, false,
         "Bai bien dai voi resort ven bien."},
        {"POI-015", "Lang Moc Kim Bong", "Kim Bong Carpentry Village", 500, 350, 15.8820, 108.3350,
         POITier::T2, POICategory::HISTORY, 75, false, 30000, 300, 1500, false,
         "Lang nghe moc truyen thong ven song Thu Bon."},
        {"POI-016", "Hoi Quan Trieu Chau", "Chaozhou Assembly Hall", 280, 110, 15.8782, 108.3287,
         POITier::T2, POICategory::SPIRITUAL, 75, false, 40000, 250, 1250, false,
         "Hoi quan nguoi Trieu Chau voi kien truc doc dao."},
        {"POI-017", "Bao Tang Gom Su", "Museum of Trade Ceramics", 320, 170, 15.8788, 108.3292,
         POITier::T3, POICategory::HISTORY, 50, false, 80000, 150, 750, false,
         "Hien vat gom su tu thoi Dai Viet."},
        {"POI-018", "Cong Vien Van Hoa", "Cultural Park", 350, 250, 15.8795, 108.3310,
         POITier::T3, POICategory::ENTERTAINMENT, 50, false, 0, 100, 500, false,
         "Khong gian van hoa va giai tri."},
        {"POI-019", "Song Thu Bon", "Thu Bon River", 200, 300, 15.8775, 108.3315,
         POITier::T2, POICategory::NATURE, 75, false, 0, 200, 1000, false,
         "Dong song bao quanh pho co, thuyen buom ven bo."},
        {"POI-020", "Nha Tho Hoc Nguyen Tuong", "Nguyen Tuong Clan House", 190, 200, 15.8776, 108.3298,
         POITier::T3, POICategory::HISTORY, 50, false, 0, 100, 500, false,
         "Nha tho dong ho Nguyen Tuong co xua."},
        {"POI-021", "Chua Cam Pho", "Cam Pho Temple", 420, 180, 15.8792, 108.3285,
         POITier::T3, POICategory::SPIRITUAL, 50, false, 0, 100, 500, false,
         "Ngoi chua Phat giao trong pho co."},
        {"POI-022", "Cho Dem Hoi An", "Hoi An Night Market", -30, 280, 15.8760, 108.3300,
         POITier::T2, POICategory::FOOD, 75, false, 0, 250, 1250, false,
         "Cho dem song dong voi am thuc duong pho."},
        {"POI-023", "Quan Cao Lau Ba Be", "Ba Be Cao Lau Shop", 80, 210, 15.8769, 108.3295,
         POITier::T2, POICategory::FOOD, 50, false, 0, 200, 1000, false,
         "Quan Cao Lau noi tieng nhat pho co."},
        {"POI-024", "Cau Buom Sang", "Morning Glory Bridge", 110, 280, 15.8772, 108.3302,
         POITier::T3, POICategory::HERITAGE, 50, false, 0, 100, 500, true,
         "Goc chup anh bi mat ven song - chi mo khoa qua quest."},
    };
}

inline std::vector<FoodItem> createFoodRegistry() {
  return {
    {"Pho Bo Ha Noi", "HN", 50000,
     {"", BuffType::STAMINA_REGEN, "Pho Bo", 0.25f, 0.10f, 7200, 7200, StackBehavior::REFRESH, {}}, false},
    {"Bun Bo Hue", "HUE", 55000,
     {"", BuffType::SPEED_BOOST, "Bun Bo Hue", 0.15f, 0.0f, 10800, 10800, StackBehavior::REFRESH, {}}, false},
    {"Cao Lau", "HOI_AN", 45000,
     {"", BuffType::EXP_BONUS, "Cao Lau", 0.30f, 0.0f, 7200, 7200, StackBehavior::NO_STACK, {"HOI_AN"}}, false},
    {"Banh Mi", "NATIONWIDE", 25000,
     {"", BuffType::STAMINA_REGEN, "Banh Mi", 0.10f, 0.10f, 3600, 3600, StackBehavior::REFRESH, {}}, true},
    {"Chao Long", "NATIONWIDE", 30000,
     {"", BuffType::STAMINA_REGEN, "Chao Long", 2.0f, 0.0f, 7200, 7200, StackBehavior::REFRESH, {}}, false},
    {"Ca Phe Sua Da", "HCMC", 28000,
     {"", BuffType::SPEED_BOOST, "Ca Phe Sua Da", 0.10f, 0.0f, 5400, 5400, StackBehavior::NO_STACK, {}}, true},
    {"Goi Cuon", "MIEN_NAM", 35000,
     {"", BuffType::PRICE_DISCOUNT, "Goi Cuon", 0.10f, 0.0f, 7200, 7200, StackBehavior::REFRESH, {}}, false},
    {"Ca Phe Trung", "HN", 40000,
     {"", BuffType::PHOTO_BONUS, "Ca Phe Trung", 1.5f, 0.0f, 3600, 3600, StackBehavior::NO_STACK, {}}, false},
  };
}

inline std::vector<VehicleConfig> createVehicleRegistry() {
    return {
        {VehicleType::BICYCLE, "Xe Dap", 25.0f, 9999.0f, 10000, true, 1},
        {VehicleType::HONDA_WAVE, "Honda Wave", 60.0f, 150.0f, 20000, true, 1},
        {VehicleType::YAMAHA_EXCITER, "Yamaha Exciter", 80.0f, 200.0f, 35000, false, 2},
        {VehicleType::HONDA_SH, "Honda SH", 70.0f, 180.0f, 45000, false, 2},
        {VehicleType::XICH_LO, "Xich Lo", 15.0f, 9999.0f, 30000, true, 2},
    };
}

inline std::vector<RentalStation> createRentalStations() {
    return {
        {"ST-01", "Tram Xe Pho Co", 60, 230, {VehicleType::BICYCLE, VehicleType::HONDA_WAVE}},
        {"ST-02", "Tram Xe An Bang", 680, 380, {VehicleType::BICYCLE, VehicleType::HONDA_WAVE, VehicleType::XICH_LO}},
    };
}

inline std::vector<NPC> createHoiAnNPCs() {
    return {
        {"NPC-HDV-01", "Anh Tuan", 3, "Huong dan vien", 95, 105,
         "POI-001", NPCActivity::WORKING, NPCMood::HAPPY,
         {"history", "hidden_spots", "local_food"},
         {"Chao mung den Hoi An, ban oi!", "Chua Cau la bieu tuong cua thanh pho nay.",
          "Ban muon toi ke ve lich su noi nay khong?"},
         {"Dang dan tour, gap lai sau nhe!"}},
        {"NPC-BA-NAM", "Ba Nam", 2, "Chu tap hoa", 45, 245,
         "POI-007", NPCActivity::WORKING, NPCMood::NEUTRAL,
         {"local_food", "market"},
         {"Chao con, can gi khong?", "Meo cua ba bi lac roi, con giup ba tim khong?"},
         {"Dang ban hang, quay lai sau!"}},
        {"NPC-CHU-BA", "Chu Ba", 2, "Cho thue xe", 55, 225,
         "ST-01", NPCActivity::WORKING, NPCMood::HAPPY,
         {"vehicles", "routes"},
         {"Can thue xe khong? Honda Wave 20k/gio!", "Xe dap re hon, 10k/gio thoi."},
         {"Dang sua xe, cho chut nhe!"}},
        {"NPC-CO-LAN", "Co Lan", 2, "Chu quan Cao Lau", 75, 205,
         "POI-023", NPCActivity::WORKING, NPCMood::HAPPY,
         {"local_food", "cao_lau"},
         {"Cao Lau Hoi An day, an thu di!", "Mon nay chi co o Hoi An thoi nhe."},
         {"Dang nau, doi chut!"}},
        {"NPC-ONBOARD", "Co Mai", 3, "Huong dan vien", 90, 100,
         "POI-001", NPCActivity::WORKING, NPCMood::HAPPY,
         {"onboarding", "history"},
         {"Chao mung ban den Hoi An!", "Hay theo co den Chua Cau nhe.",
          "Nhan nut Check-in khi den noi!"},
         {"Co dang ban, gap lai sau!"}},
    };
}

inline std::vector<QuestEntry> createQuestRegistry() {
    return {
        {"QUEST-MAIN-01", "Hoi An - Thanh Pho Ky Uc", "Kham pha 10 dia diem bieu tuong Hoi An",
         QuestType::MAIN, QuestStatus::AVAILABLE, "HOI_AN", "",
         {{QuestStepType::VISIT_COUNT, "Check-in 10 dia diem", "POI", 10, 0, false}},
         2000000, 500, 100, 1},
        {"QUEST-ONBOARD-01", "Chup 3 Buc Anh Pho Co", "Chup anh tai 3 dia diem quanh pho co",
         QuestType::MAIN, QuestStatus::LOCKED, "HOI_AN", "NPC-ONBOARD",
         {{QuestStepType::TAKE_PHOTO, "Chup anh Chua Cau", "POI-001", 1, 0, false},
          {QuestStepType::TAKE_PHOTO, "Chup anh Pho Co", "POI-008", 1, 0, false},
          {QuestStepType::TAKE_PHOTO, "Chup anh Den Long", "POI-009", 1, 0, false}},
         300000, 150, 50, 1},
        {"QUEST-SIDE-01", "Ba Nam Mat Meo", "Tim meo cua Ba Nam trong cho Hoi An",
         QuestType::SIDE, QuestStatus::AVAILABLE, "HOI_AN", "NPC-BA-NAM",
         {{QuestStepType::TALK_NPC, "Hoi 3 nguoi dan", "NPC", 3, 0, false},
          {QuestStepType::CHECKIN_POI, "Tim meo tai Cho Hoi An", "POI-007", 1, 0, false}},
         150000, 80, 30, 3},
        {"QUEST-DAILY-01", "Thu 3 Quan Ca Phe", "Ghe tham 3 quan ca phe hom nay",
         QuestType::DAILY, QuestStatus::AVAILABLE, "HOI_AN", "",
         {{QuestStepType::BUY_FOOD, "Mua do uong tai quan", "FOOD", 3, 0, false}},
         200000, 100, 20, 3},
        {"QUEST-DAILY-02", "Check-in Sang Som", "Check-in 2 dia diem truoc 10h sang",
         QuestType::DAILY, QuestStatus::AVAILABLE, "HOI_AN", "",
         {{QuestStepType::CHECKIN_POI, "Check-in 2 dia diem", "POI", 2, 0, false}},
         250000, 120, 40, 3},
        {"QUEST-WEEKLY-01", "Phuot 100km", "Di chuyen 100km bang xe may trong tuan",
         QuestType::WEEKLY, QuestStatus::AVAILABLE, "HOI_AN", "",
         {{QuestStepType::TRAVEL_DISTANCE, "Di 100km", "KM", 100, 0, false}},
         500000, 300, 50, 1},
        {"QUEST-SIDE-02", "Dac San Cao Lau", "Thu mon Cao Lau tai Hoi An",
         QuestType::SIDE, QuestStatus::AVAILABLE, "HOI_AN", "NPC-CO-LAN",
         {{QuestStepType::BUY_FOOD, "Mua Cao Lau", "Cao Lau", 1, 0, false}},
         100000, 60, 80, 3},
        {"QUEST-SIDE-03", "Lan Dau Thue Xe", "Thue xe may lan dau tien",
         QuestType::SIDE, QuestStatus::AVAILABLE, "HOI_AN", "NPC-CHU-BA",
         {{QuestStepType::RENT_VEHICLE, "Thue Honda Wave", "HONDA_WAVE", 1, 0, false}},
         80000, 50, 10, 3},
        {"QUEST-HIDDEN-01", "Goc Chup Bi Mat", "Kham pha Cau Buom Sang",
         QuestType::HIDDEN, QuestStatus::LOCKED, "HOI_AN", "",
         {{QuestStepType::CHECKIN_POI, "Tim Cau Buom Sang", "POI-024", 1, 0, false}},
         500000, 200, 100, 1},
        {"QUEST-COMM-01", "Cong Dong Chua Cau", "Cung cong dong check-in Chua Cau",
         QuestType::COMMUNITY, QuestStatus::AVAILABLE, "HOI_AN", "",
         {{QuestStepType::CHECKIN_POI, "Check-in Chua Cau", "POI-001", 1, 0, false}},
         100000, 50, 25, 3},
        {"QUEST-SIDE-04", "Lang Rau Tra Que", "Tham lang rau Tra Que",
         QuestType::SIDE, QuestStatus::AVAILABLE, "HOI_AN", "",
         {{QuestStepType::CHECKIN_POI, "Den Lang Rau Tra Que", "POI-012", 1, 0, false}},
         180000, 90, 60, 3},
        {"QUEST-SIDE-05", "Gap Go Nguoi Dan", "Noi chuyen voi 5 NPC dia phuong",
         QuestType::SIDE, QuestStatus::AVAILABLE, "HOI_AN", "",
         {{QuestStepType::TALK_NPC, "Gap 5 NPC", "NPC", 5, 0, false}},
         200000, 100, 40, 3},
    };
}

inline double distance2D(double x1, double y1, double x2, double y2) {
    return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

} // namespace vno::data
