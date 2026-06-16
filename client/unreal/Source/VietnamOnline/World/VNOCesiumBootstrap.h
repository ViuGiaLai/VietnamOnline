#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VNOCesiumBootstrap.generated.h"

/**
 * Phase A — Cesium for Unreal (doc 3.2, 658-671)
 * Spawns CesiumGeoreference + 3D Tileset at Hoi An when plugin enabled.
 * Ion token from .env via VNOEnvSettings.
 */
UCLASS()
class VIETNAMONLINE_API AVNOCesiumBootstrap : public AActor
{
    GENERATED_BODY()

public:
    AVNOCesiumBootstrap();

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "VNO|Cesium")
    double OriginLatitude = 15.8794;

    UPROPERTY(EditAnywhere, Category = "VNO|Cesium")
    double OriginLongitude = 108.3294;

    UPROPERTY(EditAnywhere, Category = "VNO|Cesium")
    double OriginHeight = 0.0;

    /** Cesium ion asset id for photorealistic 3D tiles (default Google Photorealistic 3D) */
    UPROPERTY(EditAnywhere, Category = "VNO|Cesium")
    int64 IonTilesetId = 2275207;

    UFUNCTION(BlueprintPure, Category = "VNO|Cesium")
    bool IsCesiumActive() const { return bCesiumSpawned; }

    UFUNCTION(BlueprintCallable, Category = "VNO|Cesium")
    FVector LatLngToUnreal(double Latitude, double Longitude, double HeightMeters = 0.0) const;

private:
    bool TrySpawnCesiumActors();
    void ConfigureActorProperty(AActor* Actor, const FName& PropName, const FString& Value);
    void ConfigureActorProperty(AActor* Actor, const FName& PropName, double Value);

    UPROPERTY()
    TObjectPtr<AActor> GeoreferenceActor;

    bool bCesiumSpawned = false;
};
