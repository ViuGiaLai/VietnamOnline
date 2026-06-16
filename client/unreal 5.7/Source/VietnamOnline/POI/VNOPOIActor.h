#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "vno/schemas/POI.hpp"
#include "VNOPOIActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UTextRenderComponent;
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

    UFUNCTION(BlueprintPure, Category = "VNO|POI")
    FString GetPoiCategory() const { return CategoryStr; }

    /** Color for minimap display based on category */
    UFUNCTION(BlueprintPure, Category = "VNO|POI")
    FLinearColor GetMapColor() const;

    UFUNCTION(BlueprintPure, Category = "VNO|POI")
    bool IsCheckedIn() const { return bCheckedIn; }

protected:
    void SetupVisualsByCategory(const FLinearColor& Color);
    void SetCheckedInVisuals();

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> BaseMesh;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> MarkerMesh;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UTextRenderComponent> NameLabel;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USphereComponent> CheckInRadius;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|POI")
    FString PoiId;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|POI")
    FString NameVi;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|POI")
    FString CategoryStr;

    UPROPERTY(BlueprintReadOnly, Category = "VNO|POI")
    bool bCheckedIn = false;

    int64 EntryFeeVnd = 0;
    int32 ExpReward = 0;

    FLinearColor PoiColor;
};
