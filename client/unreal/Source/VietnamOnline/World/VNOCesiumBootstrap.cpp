#include "World/VNOCesiumBootstrap.h"
#include "Core/VNOGameInstance.h"
#include "Core/VNOEnvSettings.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/UnrealType.h"

AVNOCesiumBootstrap::AVNOCesiumBootstrap()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AVNOCesiumBootstrap::BeginPlay()
{
    Super::BeginPlay();
    if (UVNOGameInstance* GI = Cast<UVNOGameInstance>(GetGameInstance()))
    {
        if (UVNOEnvSettings* Env = GI->GetEnv())
        {
            OriginLatitude = Env->GetFloat(TEXT("VNO_ZONE_HOI_AN_LAT"), 15.8794f);
            OriginLongitude = Env->GetFloat(TEXT("VNO_ZONE_HOI_AN_LNG"), 108.3294f);
        }
    }
    bCesiumSpawned = TrySpawnCesiumActors();
    if (bCesiumSpawned)
    {
        UE_LOG(LogTemp, Log, TEXT("[Cesium] Hoi An terrain active (%.4f, %.4f) — doc 3.2"), OriginLatitude, OriginLongitude);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Cesium] Plugin chua bat — chay tools/ue5/setup_phase_a.py sau khi cai CesiumForUnreal"));
    }
}

bool AVNOCesiumBootstrap::TrySpawnCesiumActors()
{
    UWorld* World = GetWorld();
    if (!World) return false;

    UClass* GeoClass = StaticLoadClass(AActor::StaticClass(), nullptr,
        TEXT("/Script/CesiumRuntime.CesiumGeoreference"));
    UClass* TileClass = StaticLoadClass(AActor::StaticClass(), nullptr,
        TEXT("/Script/CesiumRuntime.Cesium3DTileset"));
    if (!GeoClass || !TileClass) return false;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    GeoreferenceActor = World->SpawnActor<AActor>(GeoClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
    if (!GeoreferenceActor) return false;

    ConfigureActorProperty(GeoreferenceActor, TEXT("OriginLatitude"), OriginLatitude);
    ConfigureActorProperty(GeoreferenceActor, TEXT("OriginLongitude"), OriginLongitude);
    ConfigureActorProperty(GeoreferenceActor, TEXT("OriginHeight"), OriginHeight);

    AActor* Tileset = World->SpawnActor<AActor>(TileClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
    if (!Tileset) return false;

    FString Token;
    if (UVNOGameInstance* GI = Cast<UVNOGameInstance>(GetGameInstance()))
    {
        if (UVNOEnvSettings* Env = GI->GetEnv())
            Token = Env->Get(TEXT("CESIUM_ION_TOKEN"), TEXT(""));
    }
    if (!Token.IsEmpty())
        ConfigureActorProperty(Tileset, TEXT("IonAccessToken"), Token);

    ConfigureActorProperty(Tileset, TEXT("IonTilesetID"), static_cast<double>(IonTilesetId));

    if (FObjectProperty* GeoProp = FindFProperty<FObjectProperty>(Tileset->GetClass(), TEXT("Georeference")))
    {
        GeoProp->SetObjectPropertyValue_InContainer(Tileset, GeoreferenceActor);
    }

    return true;
}

void AVNOCesiumBootstrap::ConfigureActorProperty(AActor* Actor, const FName& PropName, const FString& Value)
{
    if (!Actor) return;
    if (FStrProperty* Prop = FindFProperty<FStrProperty>(Actor->GetClass(), PropName))
        Prop->SetPropertyValue_InContainer(Actor, Value);
}

void AVNOCesiumBootstrap::ConfigureActorProperty(AActor* Actor, const FName& PropName, double Value)
{
    if (!Actor) return;
    if (FDoubleProperty* Prop = FindFProperty<FDoubleProperty>(Actor->GetClass(), PropName))
        Prop->SetPropertyValue_InContainer(Actor, Value);
    else if (FFloatProperty* FProp = FindFProperty<FFloatProperty>(Actor->GetClass(), PropName))
        FProp->SetPropertyValue_InContainer(Actor, static_cast<float>(Value));
    else if (FInt64Property* IProp = FindFProperty<FInt64Property>(Actor->GetClass(), PropName))
        IProp->SetPropertyValue_InContainer(Actor, static_cast<int64>(Value));
}

FVector AVNOCesiumBootstrap::LatLngToUnreal(double Latitude, double Longitude, double HeightMeters) const
{
    if (!GeoreferenceActor) return FVector::ZeroVector;
    UFunction* Fn = GeoreferenceActor->FindFunction(TEXT("TransformLongitudeLatitudeHeightPositionToUnreal"));
    if (!Fn) return FVector(static_cast<float>(Longitude * 1000.0), static_cast<float>(Latitude * 1000.0), static_cast<float>(HeightMeters * 100.0));

    struct { FVector Llh; FVector ReturnValue; } Params;
    Params.Llh = FVector(Longitude, Latitude, HeightMeters);
    GeoreferenceActor->ProcessEvent(Fn, &Params);
    return Params.ReturnValue;
}
