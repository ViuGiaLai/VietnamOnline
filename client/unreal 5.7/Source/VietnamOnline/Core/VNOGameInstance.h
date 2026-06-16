#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Core/VNOEnvSettings.h"
#include "VNOGameInstance.generated.h"

class UVNOWorldSubsystem;
class UVNOWeatherSubsystem;
class UVNOQuestSubsystem;

UCLASS()
class VIETNAMONLINE_API UVNOGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    UFUNCTION(BlueprintPure, Category = "VNO")
    UVNOEnvSettings* GetEnv() const { return EnvSettings; }

    UFUNCTION(BlueprintPure, Category = "VNO")
    FString GetZoneId() const { return ZoneId; }

    UPROPERTY(BlueprintReadOnly, Category = "VNO")
    FString ZoneId = TEXT("HOI_AN");

private:
    UPROPERTY()
    TObjectPtr<UVNOEnvSettings> EnvSettings;
};
