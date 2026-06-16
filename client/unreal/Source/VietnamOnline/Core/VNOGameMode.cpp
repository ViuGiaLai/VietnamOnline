#include "Core/VNOGameMode.h"
#include "Core/VNOGameInstance.h"
#include "Player/VNOPlayerCharacter.h"
#include "Player/VNOPlayerController.h"
#include "Player/VNOPlayerState.h"
#include "UI/VNOHUD.h"
#include "POI/VNOPOIActor.h"
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
        false);

    GetWorldTimerManager().SetTimer(
        PlayerSpawnTimer,
        this,
        &AVNOGameMode::PlacePlayerAtZoneOrigin,
        0.6f,
        false);

    UE_LOG(LogTemp, Log, TEXT("[VNO] Phase A — Cesium + POI + NavMesh ready"));
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
