#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "VNOQuestSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FVNOQuestRuntime
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString QuestId;

    UPROPERTY(BlueprintReadOnly)
    FString Title;

    UPROPERTY(BlueprintReadOnly)
    bool bActive = false;

    UPROPERTY(BlueprintReadOnly)
    int32 StepsDone = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 StepsTotal = 1;
};

UCLASS()
class VIETNAMONLINE_API UVNOQuestSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "VNO|Quest")
    bool AcceptQuest(const FString& QuestId);

    void OnPoiCheckedIn(const FString& PoiId);
    void OnVehicleRented(const FString& VehicleId);
    void OnFoodPurchased(const FString& FoodName);
    void OnNPCTalked(const FString& NpcId, const FString& NpcName);
    void RestoreQuestProgress(const FString& QuestId, bool bActive, int32 StepsDone);
    void LogActiveQuests() const;

    UFUNCTION(BlueprintPure, Category = "VNO|Quest")
    const TArray<FVNOQuestRuntime>& GetQuests() const { return RuntimeQuests; }

private:
    UPROPERTY()
    TArray<FVNOQuestRuntime> RuntimeQuests;
};
