#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "vno/schemas/BuffEntry.hpp"
#include "vno/data/HoiAnZoneData.hpp"
#include "VNOFoodShop.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class AVNOPlayerCharacter;

USTRUCT(BlueprintType)
struct FVNOFoodOption
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString Name;

    UPROPERTY(BlueprintReadOnly)
    int64 Price = 0;

    UPROPERTY(BlueprintReadOnly)
    FString BuffDescription;

    UPROPERTY(BlueprintReadOnly)
    FString Region;

    UPROPERTY(BlueprintReadOnly)
    bool bAvailable = true;
};

UCLASS()
class VIETNAMONLINE_API AVNOFoodShop : public AActor
{
    GENERATED_BODY()

public:
    AVNOFoodShop();

    void InitFromData(const vno::FoodItem& Item, const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "VNO|Food")
    bool TryOrderFood(AVNOPlayerCharacter* Player, int32 FoodIndex);

    UFUNCTION(BlueprintPure, Category = "VNO|Food")
    FString GetShopName() const { return ShopName; }

    UFUNCTION(BlueprintPure, Category = "VNO|Food")
    const TArray<FVNOFoodOption>& GetMenu() const { return Menu; }

protected:
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> StallMesh;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UTextRenderComponent> Label;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USphereComponent> InteractRadius;

    UPROPERTY()
    TArray<FVNOFoodOption> Menu;

    // Non-UHT type — C++ only
    TArray<vno::BuffEntry> BuffData;

    FString ShopName;
    FString FoodItemName;
    int64 FoodPrice = 0;
};
