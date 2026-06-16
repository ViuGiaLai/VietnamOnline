#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VNOGameMode.generated.h"

class AVNOPOIActor;
class AVNOVehicleRentalStation;
class AVNOFoodShop;
class AVNONPCActor;

UCLASS()
class VIETNAMONLINE_API AVNOGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AVNOGameMode();

    virtual void BeginPlay() override;

protected:
    void SpawnHoiAnPOIs();
    void SpawnRentalStations();
    void SpawnFoodShops();
    void SpawnHoiAnNPCs();
    void PlacePlayerAtZoneOrigin();

    FTimerHandle PoiSpawnTimer;
    FTimerHandle RentalSpawnTimer;
    FTimerHandle FoodSpawnTimer;
    FTimerHandle NpcSpawnTimer;
    FTimerHandle PlayerSpawnTimer;

    UPROPERTY(EditDefaultsOnly, Category = "VNO|POI")
    TSubclassOf<AVNOPOIActor> POIActorClass;

    UPROPERTY(EditDefaultsOnly, Category = "VNO|Vehicles")
    TSubclassOf<AVNOVehicleRentalStation> RentalStationClass;

    UPROPERTY(EditDefaultsOnly, Category = "VNO|Food")
    TSubclassOf<AVNOFoodShop> FoodShopClass;

    UPROPERTY(EditDefaultsOnly, Category = "VNO|NPC")
    TSubclassOf<AVNONPCActor> NpcClass;

    /** 1 unit = 1m; map console coords * Scale */
    UPROPERTY(EditDefaultsOnly, Category = "VNO|World")
    float WorldScale = 1.0f;
};
