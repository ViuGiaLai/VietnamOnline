#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "vno/schemas/POI.hpp"
#include "VNOPOIActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class AVNOPlayerCharacter;

UCLASS()
class VIETNAMONLINE_API AVNOPOIActor : public AActor
{
    GENERATED_BODY()

public:
    AVNOPOIActor();

    void InitFromData(const struct vno::POI& PoiData);
    bool TryCheckIn(AVNOPlayerCharacter* Player);

    UFUNCTION(BlueprintPure, Category = "VNO|POI")
    FString GetPoiName() const { return NameVi; }

protected:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USphereComponent> CheckInRadius;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|POI")
    FString PoiId;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|POI")
    FString NameVi;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|POI")
    bool bCheckedIn = false;

    int64 EntryFeeVnd = 0;
    int32 ExpReward = 0;
};
