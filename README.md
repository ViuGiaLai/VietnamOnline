# Vietnam Online

Monorepo **Vietnam Online** — MMO du lich van hoa ao (MVP Hoi An).

Chi tiet cau truc: **[STRUCTURE.md](STRUCTURE.md)**

## Cau truc tong

```
client/     UE5 modules + prototype console (choi duoc)
server/     9 microservices MVP + 10 services Phase 2+
shared/     Schema, enum, constant, validation, network
data/       Config data-driven (POI, quest, saves)
tools/      Validate, import OSM, codegen
infra/      Docker PostgreSQL + Redis
analytics/  Event schemas (FTUE, retention)
tests/      Validation + gameplay tests
doc/        Tai lieu thiet ke goc
```

## Tich hop ha tang (doc Ch.31-35)

| Dich vu | Bien `.env` | Code |
|---------|-------------|------|
| **Supabase** | `SUPABASE_URL`, `SUPABASE_ANON_KEY`, `DATABASE_URL` | Auth FR-01, save cloud FR-10 |
| **Redis** | `REDIS_URL` | Cache weather, session CCU, rate limit |
| **OpenWeatherMap** | `OPENWEATHERMAP_API_KEY` | Thoi tiet that Hoi An (sync 15 phut) |
| **OpenStreetMap** | `OSM_OVERPASS_URL` | POI + ten duong that (FR-06) |
| **Cesium Ion** | `CESIUM_ION_TOKEN` | UE5 terrain 3D (client/unreal 5.7) |

Kiem tra ket noi:

```cmd
tools\infra\check_connections.bat
tools\supabase\check_schema.bat
```

## Supabase (cloud)

1. **SQL schema** — mo [Supabase SQL Editor](https://supabase.com/dashboard), paste noi dung `infra/database/schema_supabase.sql` va Run.
2. **`.env`** — da co `SUPABASE_URL` + `SUPABASE_ANON_KEY`. Thay `YOUR_DB_PASSWORD` trong `DATABASE_URL` bang mat khau Database (Project Settings > Database).
3. **Auth** — dat `VNO_DEV_SKIP_AUTH=false`, chay game, dang ky email/mat khau. Profile tu dong sync len bang `characters` moi 30s (FR-10).

> **Bao mat:** Khong commit `.env`. Neu da chia se key trong chat, nen rotate key trong Supabase Dashboard.

## Cau hinh moi truong (.env)

```bash
cp .env.example .env
# Chinh sua DATABASE_URL, SUPABASE_*, JWT secret...
```

Bien quan trong (theo doc Ch.31-35):

| Bien | Muc dich |
|------|----------|
| `VNO_DEV_SKIP_AUTH` | `true` = bo qua dang nhap khi dev |
| `VNO_CHECKPOINT_SAVE_SEC` | Auto-save moi 30s (FR-10) |
| `VNO_PROXIMITY_CHAT_RADIUS_M` | Ban kinh chat 50m (FR-05) |
| `DATABASE_URL` | PostgreSQL / Supabase |
| `VNO_API_BASE_URL` | Gateway server |

Khoi dong DB local:

```bash
cd infra/docker && docker compose up -d
```

## Build nhanh (Windows)

```cmd
build_and_run.bat
```

Tao ra:
- `build/vno_prototype.exe` — game console MVP
- `build/vno_server.exe` — server MVP 9 services

## Build (Linux/macOS)

```bash
make          # prototype + server + tests
make run      # choi prototype
```

## Choi thu

### Console MVP (logic nhanh)
```cmd
build\vno_prototype.exe
```

### UE5 3D Desktop (dung tai lieu thiet ke)
Xem **[client/unreal 5.7/README.md](client/unreal%205.7/README.md)** — mo `VietnamOnline.uproject` trong Unreal Engine 5.7+, tao map, nhan **Play**.

Lenh console: `help`, `w/a/s/d`, `go 100 100`, `e` check-in, `rent`, `j` quest, `m` minimap

## MVP map voi tai lieu

| FR | Module |
|----|--------|
| FR-01 | `server/auth-service` |
| FR-03/04 | `client/.../Movement`, `server/movement-sync-service` |
| FR-05 | `server/chat-service`, `client/.../Social` |
| FR-07 | `server/poi-service`, `data/zones/hoi_an` |
| FR-08 | `server/quest-service` |
| FR-09 | `server/vehicle-service`, `client/.../Vehicles` |
| FR-10 | `server/profile-service`, `client/.../SaveGame` |

Shared contracts: `#include "vno/VNO.hpp"`
