# Vietnam Online — UE5 Client 3D (doc v1.0)

Game desktop 3D theo tai lieu: third-person, HUD 4 goc, POI check-in, quest, OpenWeatherMap, Cesium terrain Hoi An.

## Yeu cau

| Phan mem | Phien ban |
|----------|-----------|
| Unreal Engine | **5.3+** |
| Visual Studio | 2022 (C++ workload) |
| Epic Launcher | Cai UE5 |

## Giai doan A — Choi duoc tren desktop (checklist)

### Buoc 1: Mo project va compile

1. Double-click `client/unreal/VietnamOnline.uproject`
2. Chon **Yes** neu hoi generate Visual Studio project files
3. Build **Development Editor** (VS hoac Compile trong Editor)

### Buoc 2: Cau hinh `.env`

Copy `.env.example` → `.env` o thu muc `VietnamOnline/` va dien:

| Bien | Muc dich |
|------|----------|
| `SUPABASE_URL`, `SUPABASE_ANON_KEY` | FR-01 Auth + FR-10 Save |
| `CESIUM_ION_TOKEN` | Terrain 3D Hoi An |
| `OPENWEATHERMAP_API_KEY` | Thoi tiet HUD |
| `VNO_DEV_SKIP_AUTH=true` | Bo qua login khi dev (tuy chon) |
| `VNO_CHECKPOINT_SAVE_SEC=30` | Auto-save (FR-10) |

### Buoc 3: Supabase schema (FR-10)

1. Mo [Supabase SQL Editor](https://supabase.com/dashboard)
2. Chay noi dung file `infra/database/schema_supabase.sql`
3. Kiem tra bang `characters` da tao

### Buoc 4: Cai Cesium for Unreal

1. Epic Marketplace → **Cesium for Unreal** → Install
2. Mo lai project → bat plugin `CesiumForUnreal` (hoac script tu bat)
3. Token Ion tu `.env` (`CESIUM_ION_TOKEN`)

### Buoc 5: Setup map (quan trong)

Trong Unreal Editor:

1. **Tools → Execute Python Script**
2. Chon `tools/ue5/setup_phase_a.py`

Script tu dong:

- Tao/cap nhat map `Content/Maps/HoiAn_MVP`
- Anh sang Lumen, NavMeshBoundsVolume, PlayerStart
- Nen Plane tam (neu chua co Cesium)
- Spawn CesiumGeoreference + 3D Tileset (15.8794, 108.3294)
- Build NavMesh
- Copy Mannequin mesh → `/Game/VNO/Characters/Mesh/SKM_VNOPlayer`

### Buoc 6: Play

Nhan **Play (Alt+P)** — cua so game 3D desktop.

## Dieu khien

| Phim | Chuc nang |
|------|-----------|
| W/A/S/D | Di chuyen (FR-03) |
| Mouse | Xoay camera |
| Shift | Sprint (stamina) |
| E | Check-in POI gan nhat (FR-07) |
| F8 | Photo Mode |
| M | Bat/tat minimap (FR-06) |
| J | Quest journal (FR-08) |

### Auth & tao nhan vat (FR-01, FR-02)

Mo **Output Log** hoac console trong game (`~`):

```
vno.Login email@example.com matkhau
vno.SignUp email@example.com matkhau
```

| Khi tao nhan vat: **1/2** gioi tinh, **Q/W** tone da, **Enter** xac nhan.

Voi `VNO_DEV_SKIP_AUTH=true`: vao thang man tao nhan vat, save local + cloud.

## Tich hop ky thuat (Giai doan A)

| FR | Module | Mo ta |
|----|--------|-------|
| FR-01 | `VNOSupabaseSubsystem` | SignIn/SignUp REST |
| FR-02 | `VNOCharacterCreateWidget` | Ten, gioi tinh, tone da |
| FR-03 | `VNOPlayerCharacter` + NavMesh | WASD, sprint, Mannequin, `VNOAnimInstance` |
| FR-07 | `VNOPOIActor` | 24 POI spawn GPS/console coords |
| FR-08 | `VNOQuestSubsystem` | 12 quest, accept MAIN-01 |
| FR-10 | `VNOSupabaseSubsystem` | Auto-save 30s, load cloud |

| He thong | File |
|----------|------|
| Cesium terrain | `VNOCesiumBootstrap` — spawn runtime neu plugin bat |
| .env | `VNOEnvSettings` doc `../../.env` |
| Thoi tiet | `VNOWeatherSubsystem` — OpenWeatherMap |

## Cau truc code

```
Source/VietnamOnline/
  Core/       VNOGameInstance, VNOGameMode, VNOEnvSettings
  Player/     Character, Controller, PlayerState, AnimInstance
  POI/        VNOPOIActor — 24 dia diem
  Quest/      VNOQuestSubsystem
  SaveGame/   VNOSupabaseSubsystem
  UI/         VNOHUD, VNOCharacterCreateWidget
  World/      VNOCesiumBootstrap, VNOWorldSubsystem
  Weather/    VNOWeatherSubsystem
```

## Loi thuong gap

| Loi | Cach sua |
|-----|----------|
| Compile fail | VS2022 + Windows SDK, regenerate project files |
| Map khong load | Chay `setup_phase_a.py` |
| Khong thay terrain | Cai + bat CesiumForUnreal, nhap `CESIUM_ION_TOKEN` |
| Save cloud fail | Chay `schema_supabase.sql`, kiem tra Supabase URL/key |
| Khong co Mannequin | Add Feature: Third Person (Content Only) trong Editor |
| POI khong thay | Doi 0.5s sau Play — spawn sau Cesium init |

## Tiep theo (Giai doan B)

- FR-04 Multiplayer WebSocket sync
- FR-05 Chat proximity server
- Port quest/xe/chat day du tu console sang UE5
