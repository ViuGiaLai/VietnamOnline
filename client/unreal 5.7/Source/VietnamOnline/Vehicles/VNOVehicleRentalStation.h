#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "vno/schemas/Vehicle.hpp"
#include "vno/data/HoiAnZoneData.hpp"
#include "VNOVehicleRentalStation.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class AVNOPlayerCharacter;

USTRUCT(BlueprintType)
struct FVNOVehicleOption
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString Name;

    UPROPERTY(BlueprintReadOnly)
    int64 CostPerHour = 0;

    UPROPERTY(BlueprintReadOnly)
    float MaxSpeedKmh = 0.f;

    UPROPERTY(BlueprintReadOnly)
    bool bAlleyAccess = true;

    UPROPERTY(BlueprintReadOnly)
    bool bAvailable = true;
};

/** FR-09 — Rental station where players rent bicycles / motorbikes */
UCLASS()
class VIETNAMONLINE_API AVNOVehicleRentalStation : public AActor
{
    GENERATED_BODY()

public:
    AVNOVehicleRentalStation();

    void InitFromData(const vno::RentalStation& Data);
    void RefreshAvailableVehicles();

    UFUNCTION(BlueprintCallable, Category = "VNO|Vehicle")
    bool TryRentVehicle(AVNOPlayerCharacter* Player, int32 VehicleIndex);

    UFUNCTION(BlueprintPure, Category = "VNO|Vehicle")
    FString GetStationName() const { return StationName; }

    UFUNCTION(BlueprintPure, Category = "VNO|Vehicle")
    const TArray<FVNOVehicleOption>& GetVehicles() const { return AvailableVehicles; }

protected:
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
    virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USphereComponent> InteractionRadius;

    UPROPERTY()
    TArray<FVNOVehicleOption> AvailableVehicles;

    FString StationName;
    FString StationId;
    double LocX = 0.0, LocY = 0.0;
};
