#include "Core/VNOGameMode.h"
#include "Core/VNOGameInstance.h"
#include "Player/VNOPlayerCharacter.h"
#include "Player/VNOPlayerController.h"
#include "Player/VNOPlayerState.h"
#include "UI/VNOHUD.h"
#include "POI/VNOPOIActor.h"
#include "Vehicles/VNOVehicleRentalStation.h"
#include "Food/VNOFoodShop.h"
#include "NPC/VNONPCActor.h"
#include "World/VNOCesiumBootstrap.h"
#include "vno/data/HoiAnZoneData.hpp"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AVNOGameMode::AVNOGameMode()
{
    DefaultPawnClass = AVNOPlayerCharacter::StaticClass();
    PlayerControllerClass = AVNOPlayerController::StaticClass();
    PlayerStateClass = AVNOPlayerState::StaticClass();
    HUDClass = AVNOHUD::StaticClass();

    RentalStationClass = AVNOVehicleRentalStation::StaticClass();
}

void AVNOGameMode::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (!World) return;

    bool HasCesium = false;
    for (TActorIterator<AVNOCesiumBootstrap> It(World); It; ++It)
    {
        HasCesium = true;
        break;
    }
    if (!HasCesium)
    {
        World->SpawnActor<AVNOCesiumBootstrap>(AVNOCesiumBootstrap::StaticClass());
    }

  GetWorldTimerManager().SetTimer(
        PoiSpawnTimer,
        this,
        &AVNOGameMode::SpawnHoiAnPOIs,
        0.5f,
        false);    GetWorldTimerManager().SetTimer(
        RentalSpawnTimer,
        this,
        &AVNOGameMode::SpawnRentalStations,
        0.7f,
        false);

    GetWorldTimerManager().SetTimer(
        FoodSpawnTimer,
        this,
        &AVNOGameMode::SpawnFoodShops,
        0.9f,
        false);

    GetWorldTimerManager().SetTimer(
        NpcSpawnTimer,
        this,
        &AVNOGameMode::SpawnHoiAnNPCs,
        1.0f,
        false);

    GetWorldTimerManager().SetTimer(
        PlayerSpawnTimer,
        this,
        &AVNOGameMode::PlacePlayerAtZoneOrigin,
        1.2f,
        false);

    UE_LOG(LogTemp, Log, TEXT("[VNO] Phase A — Cesium + POI + Rental + Food + NavMesh ready"));
}

void AVNOGameMode::PlacePlayerAtZoneOrigin()
{
    UWorld* World = GetWorld();
    if (!World) return;

    AVNOCesiumBootstrap* Cesium = nullptr;
    for (TActorIterator<AVNOCesiumBootstrap> It(World); It; ++It)
    {
        Cesium = *It;
        break;
    }

    FVector SpawnLoc(0.f, 0.f, 200.f);
    if (Cesium && Cesium->IsCesiumActive())
    {
        SpawnLoc = Cesium->LatLngToUnreal(Cesium->OriginLatitude, Cesium->OriginLongitude, 5.0);
        UE_LOG(LogTemp, Log, TEXT("[VNO] Spawn player tai Hoi An (Cesium)"));
    }

    if (APlayerStart* Start = Cast<APlayerStart>(UGameplayStatics::GetActorOfClass(World, APlayerStart::StaticClass())))
    {
        Start->SetActorLocation(SpawnLoc);
    }

    if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0))
    {
        Pawn->SetActorLocation(SpawnLoc);
    }
}

void AVNOGameMode::SpawnRentalStations()
{
    if (!RentalStationClass) RentalStationClass = AVNOVehicleRentalStation::StaticClass();
    UWorld* World = GetWorld();
    if (!World) return;

    const auto StationList = vno::data::createRentalStations();
    int32 Spawned = 0;
    for (const vno::RentalStation& S : StationList)
    {
        FVector Loc(static_cast<float>(S.x) * WorldScale,
                    static_cast<float>(S.y) * WorldScale, 50.f);
        FActorSpawnParameters Params;
        AVNOVehicleRentalStation* Actor = World->SpawnActor<AVNOVehicleRentalStation>(
            RentalStationClass, Loc, FRotator::ZeroRotator, Params);
        if (Actor)
        {
            Actor->InitFromData(S);
            Spawned++;
        }
    }
    UE_LOG(LogTemp, Log, TEXT("[FR-09] Spawned %d rental stations"), Spawned);
}

void AVNOGameMode::SpawnFoodShops()
{
    if (!FoodShopClass) FoodShopClass = AVNOFoodShop::StaticClass();
    UWorld* World = GetWorld();
    if (!World) return;

    const auto FoodRegistry = vno::data::createFoodRegistry();
    const auto PoiList = vno::data::createHoiAnPOIs();

    // Food shop spawn locations: near FOOD-category POIs + some along main street
    TArray<FVector> SpawnPoints;
    for (const vno::POI& P : PoiList)
    {
        if (P.category == vno::POICategory::FOOD)
        {
            SpawnPoints.Add(FVector(static_cast<float>(P.x) * WorldScale,
                                     static_cast<float>(P.y) * WorldScale, 50.f));
        }
    }
    // Add street-side food spots
    SpawnPoints.Add(FVector(130.f, 150.f, 50.f));
    SpawnPoints.Add(FVector(300.f, 100.f, 50.f));
    SpawnPoints.Add(FVector(500.f, 200.f, 50.f));
    SpawnPoints.Add(FVector(-50.f, 100.f, 50.f));
    SpawnPoints.Add(FVector(400.f, 50.f, 50.f));

    int32 Spawned = 0;
    for (int32 i = 0; i < FoodRegistry.size() && i < SpawnPoints.Num(); ++i)
    {
        const vno::FoodItem& Item = FoodRegistry[i];
        FActorSpawnParameters Params;
        AVNOFoodShop* Actor = World->SpawnActor<AVNOFoodShop>(
            FoodShopClass, SpawnPoints[i], FRotator::ZeroRotator, Params);
        if (Actor)
        {
            Actor->InitFromData(Item, SpawnPoints[i]);
            Spawned++;
            UE_LOG(LogTemp, Log, TEXT("[FR-12] Food shop: %s — %lld VND"),
                UTF8_TO_TCHAR(Item.name.c_str()), Item.price);
        }
    }
    UE_LOG(LogTemp, Log, TEXT("[FR-12] Spawned %d food shops"), Spawned);
}

void AVNOGameMode::SpawnHoiAnNPCs()
{
    if (!NpcClass) NpcClass = AVNONPCActor::StaticClass();
    UWorld* World = GetWorld();
    if (!World) return;

    const auto NpcList = vno::data::createHoiAnNPCs();
    int32 Spawned = 0;
    for (const vno::NPC& N : NpcList)
    {
        FVector Loc(static_cast<float>(N.x) * WorldScale,
                    static_cast<float>(N.y) * WorldScale, 50.f);
        FActorSpawnParameters Params;
        AVNONPCActor* Actor = World->SpawnActor<AVNONPCActor>(
            NpcClass, Loc, FRotator::ZeroRotator, Params);
        if (Actor)
        {
            Actor->InitFromData(N);
            Spawned++;
            UE_LOG(LogTemp, Log, TEXT("[FR-10] NPC: %s (%s) tai (%.0f, %.0f)"),
                UTF8_TO_TCHAR(N.name.c_str()),
                UTF8_TO_TCHAR(N.job.c_str()),
                N.x, N.y);
        }
    }
    UE_LOG(LogTemp, Log, TEXT("[FR-10] Spawned %d NPCs"), Spawned);
}

void AVNOGameMode::SpawnHoiAnPOIs()
{
    if (!POIActorClass) POIActorClass = AVNOPOIActor::StaticClass();
    UWorld* World = GetWorld();
    if (!World) return;

    AVNOCesiumBootstrap* Cesium = nullptr;
    for (TActorIterator<AVNOCesiumBootstrap> It(World); It; ++It)
    {
        Cesium = *It;
        break;
    }

    const std::vector<vno::POI> PoiList = vno::data::createHoiAnPOIs();
    int32 Spawned = 0;
    for (const vno::POI& P : PoiList)
    {
        if (P.is_hidden) continue;

        FVector Loc;
        if (Cesium && Cesium->IsCesiumActive() && P.lat != 0.0)
        {
            Loc = Cesium->LatLngToUnreal(P.lat, P.lng, 2.0);
        }
        else
        {
            Loc = FVector(static_cast<float>(P.x) * WorldScale,
                          static_cast<float>(P.y) * WorldScale, 120.f);
        }

        FActorSpawnParameters Params;
        AVNOPOIActor* Actor = World->SpawnActor<AVNOPOIActor>(POIActorClass, Loc, FRotator::ZeroRotator, Params);
        if (Actor)
        {
            Actor->InitFromData(P);
            Spawned++;
        }
    }
    UE_LOG(LogTemp, Log, TEXT("[VNO] Spawned %d POIs (FR-07)"), Spawned);
}
