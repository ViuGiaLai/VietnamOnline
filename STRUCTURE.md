# Vietnam Online — Cấu trúc codebase

Monorepo theo tài liệu thiết kế v1.0 (MVP Hội An).

## 1. Tổng thể

```text
VietnamOnline/
├── client/           # Game client (UE5 + prototype console)
├── server/           # Backend microservices
├── shared/           # Schema / enum / constant / validation / network
├── data/             # Config data-driven (POI, quest, economy...)
├── tools/            # Validate, import OSM, codegen
├── infra/            # Docker, CI, monitoring
├── analytics/        # Event schemas, retention KPI
├── tests/            # Unit / gameplay / server tests
└── doc/              # Tài liệu thiết kế gốc
```

## 2. Client (`client/`)

### 2.1 Prototype chơi được (MVP logic)
```text
client/prototype/
├── include/
│   ├── GameEngine.hpp      # Game loop tích hợp mọi system MVP
│   └── PlayerDisplay.hpp   # UI console
└── src/
    ├── main.cpp
    └── GameEngine.cpp
```
Build: `build/vno_prototype.exe`

### 2.2 Unreal Engine 5
```text
client/unreal/
├── VietnamOnline.uproject
└── Source/VietnamOnline/
    ├── Core/              # Game instance, bootstrap zone
    ├── Player/            # Avatar, state machine, buff
    ├── Input/             # WASD / gamepad / mobile bindings
    ├── Movement/          # Walk, sprint, stamina
    ├── Vehicles/          # Rental, fuel, physics hook
    ├── POI/               # Check-in, journal, hidden POI
    ├── Quest/             # Main/side/daily tracker
    ├── Economy/           # VND, Xu Vàng UI
    ├── Social/            # Chat bubble, party, friend
    ├── NPC/               # Dialogue L1/L2, AI hook L3
    ├── World/             # Zone load, minimap, OSM/Cesium hook
    ├── Weather/           # Visual + gameplay modifier
    ├── UI/                # HUD 4 góc, quest journal
    ├── PhotoMode/         # F8 camera, ELO preview
    ├── Audio/             # Ambient, adaptive music
    ├── SaveGame/          # Local cache, reconnect
    ├── Multiplayer/       # Replication, ping, prediction
    ├── Accessibility/     # WCAG AA options
    └── Analytics/         # FTUE / retention events
```
Mỗi module có `VNO{Module}Module.h/.cpp` stub, include `shared/include/vno/`.

## 3. Server (`server/`) — MVP Phase 1

| Service | Thư mục | FR |
|---------|---------|-----|
| gateway | `gateway/` | Routing, rate limit |
| auth-service | `auth-service/` | FR-01 |
| profile-service | `profile-service/` | FR-10 save/load |
| world-service | `world-service/` | Weather, zone time |
| movement-sync-service | `movement-sync-service/` | FR-03, FR-04 |
| chat-service | `chat-service/` | FR-05 |
| poi-service | `poi-service/` | FR-07 |
| quest-service | `quest-service/` | FR-08 |
| vehicle-service | `vehicle-service/` | FR-09 |

Phase 2+: `zone-instance-service`, `social-service`, `economy-service`, `booking-service`, `npc-service`, `npc-ai-service`, `photo-service`, `analytics-service`, `moderation-service`, `admin-gm-service`

Build: `build/vno_server.exe`

## 4. Shared (`shared/include/vno/`)

| Thư mục | Nội dung |
|---------|----------|
| `enums/` | MovementState, QuestType, WeatherType, ChatChannel... |
| `schemas/` | PlayerEntity, POI, QuestEntry, ChatMessage, WeatherState... |
| `constants/` | Speed, stamina, sync tick, economy caps |
| `validation/` | Anti-cheat, check-in radius, wallet rules |
| `network/` | Protocol, DTOs (UDP/WS/HTTPS) |
| `localization/` | keys_vi.json, keys_en.json |
| `data/` | HoiAnZoneData.hpp (24 POI, 12 quest, NPC, food, vehicle) |

Include umbrella: `#include "vno/VNO.hpp"`

## 5. Data (`data/`)

```text
data/
├── zones/hoi_an/       # POI, quest, NPC per zone
├── economy/            # Pricing, tax rules
├── weather/            # Default / override
└── saves/              # Player save files (prototype)
```

## 6. Tools / Infra / Analytics / Tests

- `tools/validate/validate_data.py` — kiểm tra JSON
- `tools/import_osm/` — pipeline OSM → zone (Phase 2)
- `infra/docker/docker-compose.yml` — PostgreSQL + Redis
- `analytics/schemas/events.json` — FTUE, retention events
- `tests/validation/test_validation.cpp` — shared validation rules

## 7. Build

**Windows:** `build_and_run.bat`  
**Linux/macOS:** `make` → `make run`

```bash
make prototype   # client console MVP
make server      # MVP backend
make test        # validation tests
```
