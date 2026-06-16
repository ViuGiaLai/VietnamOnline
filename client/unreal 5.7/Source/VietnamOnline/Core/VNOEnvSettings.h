#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VNOEnvSettings.generated.h"

/** Loads KEY=VALUE from monorepo .env (doc Ch.35) */
UCLASS(BlueprintType)
class VIETNAMONLINE_API UVNOEnvSettings : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "VNO|Config")
    bool LoadEnvFile();

    UFUNCTION(BlueprintPure, Category = "VNO|Config")
    FString Get(const FString& Key, const FString& DefaultValue = TEXT("")) const;

    UFUNCTION(BlueprintPure, Category = "VNO|Config")
    bool GetBool(const FString& Key, bool bDefault = false) const;

    UFUNCTION(BlueprintPure, Category = "VNO|Config")
    float GetFloat(const FString& Key, float DefaultValue = 0.f) const;

private:
    UPROPERTY()
    TMap<FString, FString> Vars_;
};
