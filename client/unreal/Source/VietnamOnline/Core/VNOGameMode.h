#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VNOGameMode.generated.h"

class AVNOPOIActor;

UCLASS()
class VIETNAMONLINE_API AVNOGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AVNOGameMode();

    virtual void BeginPlay() override;

protected:
    void SpawnHoiAnPOIs();
    void PlacePlayerAtZoneOrigin();

    FTimerHandle PoiSpawnTimer;
    FTimerHandle PlayerSpawnTimer;

    UPROPERTY(EditDefaultsOnly, Category = "VNO|POI")
    TSubclassOf<AVNOPOIActor> POIActorClass;

    /** 1 unit = 1m; map console coords * Scale */
    UPROPERTY(EditDefaultsOnly, Category = "VNO|World")
    float WorldScale = 1.0f;
};
